#!/usr/bin/perl -w

#######################################################################
# bnc-check.pl -> checks a bnc list for connectivity using ncftpls    #
# Args : <ncftpls location> <user> <pass> <timeout> <bnc1> [bnc2] ... #
#######################################################################

use strict;

require 'sys/syscall.ph';

use Net::Ping;

die("Wrong argument-number recieved.\n") unless (@ARGV == 5); 

my ($ncftpls, $username, $password, $timeout) = @ARGV;
my @BNCs = split(/ /, $ARGV[4]);

my $TIMEVAL_T = "LL";

my $i = 0;
foreach (@BNCs) {					# for each bouncer
	$i++;

	if (!/[a-zA-Z]+:[^:]+:[^:]+/) {	# bouncer format check
		print("Bouncer entry #$i ('$_') is not using correct syntax (countrycode:host:port)\n");
		next;
	}

	my ($country, $host, $port) = split(/:/, $_);

	my $tiStart = my $tiFinish = pack($TIMEVAL_T, ());
	syscall(&SYS_gettimeofday, $tiStart, 0) != -1 or die "gettimeofday: $!";

	my $reply = `$ncftpls -P $port -u $username -p $password -t $timeout -r 0 ftp://$host 2>&1`;
	
	syscall(&SYS_gettimeofday, $tiFinish, 0) != -1 or die "gettimeofday: $!";
	my @tiStart = unpack($TIMEVAL_T, $tiStart);
	my @tiFinish = unpack($TIMEVAL_T, $tiFinish);

	# fix microseconds
	for ($tiFinish[1], $tiStart[1]) { $_ /= 1_000_000 }
	my $logintime = sprintf "%.4f", (($tiFinish[0]  + $tiFinish[1]  ) - ($tiStart[0] + $tiStart[1] )) * 100;

	if ($?) {					# returned an error code...so pattern match the
								# STDOUT & STDERR in order to find out what was
								# wrong, made slightly more complex than needed
								# as ncftpls' error codes for login issues aren't
								# that useful.
		my $error;
		$_ = $reply; # heh, nasty but removes $reply =~ on the ifs ;)
		if (/username and\/or password was not accepted for login\./) {
			$error = "Couldn't login";
		} elsif (/Connection refused\./) {
			$error = "Connection refused";
		} elsif (/try again later: Connection timed out\./) {
			$error = "Connection timed out";
		} elsif (/timed out while waiting for server response\./) {
			$error = "No response"
		} elsif (/Remote host has closed the connection\./) {
			$error = "Connection lost";
		} elsif (/unknown host\./) {
			$error = "Unknown host?";
		} elsif (/Remote host has closed the connection\./) {
			$error = "Connection lost";
		} elsif (/unknown host./) {
			$error = "Unknown host?";
		} else {
			$error = "Unhandled error-type?";
		}
		print( "$i. .$country - $host:$port - DOWN ($error)\n" );

	} else {						# returned 0, so presuming all was well...
		my $p = Net::Ping->new("tcp");	# so let's ping the host to get another time ;-)


		$tiStart = $tiFinish = pack($TIMEVAL_T, ());
		syscall(&SYS_gettimeofday, $tiStart, 0) != -1 or die "gettimeofday: $!";

		$p->ping($host, 1);

		syscall(&SYS_gettimeofday, $tiFinish, 0) != -1 or die "gettimeofday: $!";
		@tiStart = unpack($TIMEVAL_T, $tiStart);
		@tiFinish = unpack($TIMEVAL_T, $tiFinish);

		# fix microseconds
		for ($tiFinish[1], $tiStart[1]) { $_ /= 1_000_000 }
		my $pingtime = sprintf "%.0f", (($tiFinish[0] + $tiFinish[1]) - ($tiStart[0] + $tiStart[1])) * 100;

		printf("%d. .%s - %s - UP (login: %sms, ping: %sms)\n", $i, $country, "$host:$port", $logintime, $pingtime);
	}
}

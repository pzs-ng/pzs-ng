#!/usr/bin/perl -w

#######################################################################
# bnc-check.pl -> checks a bnc list for connectivity using ncftpls    #
# Args : <ncftpls location> <user> <pass> <timeout> <bnc1> [bnc2] ... #
#######################################################################

#use strict;

#use Time::HiRes qw( gettimeofday );
require 'sys/syscall.ph';

use Net::Ping;

die( "Wrong arguments specified?\n" ) unless ( @ARGV == 5 ); 

my( $ncftpls, $username, $password, $timeout, @BNCs, $country, $host, $port, $reply, $tiStart, $tiFinish, $logintime, $i, $error, @pinginfo, $p, @tiStart, @tiFinish );

$i = 0;

($ncftpls, $username, $password, $timeout) = @ARGV;

@BNCs = 	split( / /, $ARGV[4] );
$TIMEVAL_T = "LL";

foreach (@BNCs) {					# for each bouncer
	$i++;
	if ( m/[a-zA-Z]+:[^:]+:[^:]+/ ) {	# bouncer format check
		($country, $host, $port) = split(/:/, $_);
	} else {
		print( "$i. Bouncer entry '$_' is not using correct syntax (countrycode:host:port)\n" );
		next;
	}

	$tiStart = $tiFinish = pack($TIMEVAL_T, ());
	syscall(&SYS_gettimeofday, $tiStart, 0) != -1 or die "gettimeofday: $!";

#	$tiStart = gettimeofday;		# time the ncftpls execution
	$reply = `$ncftpls -P $port -u $username -p $password -t $timeout -r 0 ftp://$host 2>&1`;
	
	syscall(&SYS_gettimeofday, $tiFinish, 0) != -1 or die "gettimeofday: $!";
	@tiStart = unpack($TIMEVAL_T, $tiStart);
	@tiFinish  = unpack($TIMEVAL_T, $tiFinish);

	# fix microseconds
	for ($tiFinish[1], $tiStart[1]) { $_ /= 1_000_000 }
		$logintime = sprintf "%.4f", (($tiFinish[0]  + $tiFinish[1]  ) - ($tiStart[0] + $tiStart[1] )) * 100;

#	$tiFinish = gettimeofday;
#	$logintime = ($tiFinish - $tiStart) * 1000;	# work out how many ms it took to login

	if ($?) {						# returned an error code...so pattern match the
								# STDOUT & STDERR in order to find out what was
								# wrong, made slightly more complex than needed
								# as ncftpls' error codes for login issues aren't
								# that useful.
		if ( $reply =~ m/username and\/or password was not accepted for login\./ )

		{
			$error = "Couldn't login";
		}
		elsif ( $reply =~ m/Connection refused\./ )
		{
			$error = "Connection Refused";
		}
		elsif ( $reply =~ m/try again later: Connection timed out\./ )
		{
			$error = "Connection Timed Out";
		}
		elsif ( $reply =~ m/timed out while waiting for server response\./ )
		{
			$error = "No response"
		}
		elsif ( $reply =~ m/Remote host has closed the connection\./ )
		{
			$error = "Connection Lost";
		}
		elsif ( $reply =~ m/unknown host./ )
		{
			$error = "Unknown Host?";
		}
		else {
			$error = "Unhandled Error Type?";
		}
		print( "$i. .$country - $host:$port - DOWN ($error)\n" );

	} else {						# returned 0, so presuming all was well...

		$p = Net::Ping->new("tcp");	# so let's ping the host to get another
#		$p->hires();					# bouncer speed indicator


		$tiStart = $tiFinish = pack($TIMEVAL_T, ());
		syscall(&SYS_gettimeofday, $tiStart, 0) != -1 or die "gettimeofday: $!";

		@pinginfo = $p->ping( $host, 1 );

		syscall(&SYS_gettimeofday, $tiFinish, 0) != -1 or die "gettimeofday: $!";
		@tiStart = unpack($TIMEVAL_T, $tiStart);
		@tiFinish  = unpack($TIMEVAL_T, $tiFinish);

		# fix microseconds
		for ($tiFinish[1], $tiStart[1]) { $_ /= 1_000_000 }
			$pingtime = sprintf "%.4f", (($tiFinish[0]  + $tiFinish[1]  ) - ($tiStart[0] + $tiStart[1] )) * 100;

		printf( "%d. .%s - %s - UP (login: %sms, ping: %sms)\n", $i, $country, "$host:$port", $logintime, $pingtime);

	}
}

#!/usr/bin/perl

## Made by PnG 2002 GPL (png@strippa.net)
## ver 1.3
##
## Please DO read through the settings below, things
## will work better if you do. If you want help, join 
## #getnfoscript on efnet and talk to either me (PnG), NiDO
## or ebzoner. But please, PLEASE read the readme first. If
## you are lame, dont even bother trying to install this.
## 
## BIG THANKS to: 
## ebzoner 	(ASCii ARTIST & Bash)
## NiDO 	(Bash)
##
## PETTY KEWL!
## 
#
##################################
## Settings			##
##################################################################################################################
##														##
## Do you want me to append IMDB info to your .message file? (1=yes, 0=no)					##
$APPENDIMDB = 0;												##
##														##
## Template file to use for message										##
$TEMPLATE = "imdbmessage.tml";											##
##														##
## Can contain ?time, ?title, ?name,?genre, ?rating, ?plot, ?business						##
## please use \ before every " sign. Read more about this in the README						##
$LOGFORMAT = "?time IMDB: \"?title\" \"?name\" \"?genre\" \"?rating\" \"?url\" \"?plot\" \"?business\"";	##
##														##
## This is how it will print out data when triggerd (either in shell or from the eggdrop tcl) 			##
## You can use the same flags as above, and \002 is bold.							##
$TRIGGERFORMAT = "- Info on: \002?title\002 (Directed by: \002?name\002 Genre: \002?genre\002 Rating: \002?rating\002) url: ?url\n\002Plot:\002 ?plot\n?business";
##														##
## Path to your glftpd conf											##
$PATH_TO_GLFTPD_LOG = "/glftpd/ftp-data/logs/glftpd.log";							##
##														##
## Path to your logfile	were imdb urls is stored								##
$nfofile = "/glftpd/ftp-data/logs/imdburl.log";									##
##														##
## Ignore dirs, relative to site dir										##
$IGNOREDIR1 = "Groups";												##
$IGNOREDIR2 = "Admin";												##
$IGNOREDIR3 = "pre";												##
##														##
##################################################################################################################
## SCRiPT ###
use LWP::Simple;
$time = scalar localtime(time);
$TmP4 = $LOGFORMAT;
if ($ARGV[0] ne "-f") {&readurl} else {
	if ($ARGV[0] eq "-f") {
		if ($ARGV[1] eq "") {
			print "\n:: getnfo help ::\n\n";
			print "Syntax:\n";
			print "./getnfo.pl -f <IMDBNUMBER/NAME>\n";
			print "-f = find\n";
		} else {
			&findnprint;
		}
};
sub trunct {
open NFO, "> $nfofile";
print NFO "";
close NFO;
};
############
### Subs ###
sub readurl {
	open NFO, "< $nfofile";
	@NFO = <NFO>;
	close NFO;
	foreach  $thing (@NFO) {
		($pathtorel, $imdburl) = split("OOOO", $thing);
		$pathtorel =~ s/\"//g;
		$pathtorel =~ s/IMDB: //;
		($crp, $site, $dir) = split("/", $pathtorel);
		if ($dir ne $IGNOREDIR1 and $dir ne $IGNOREDIR2 and $dir ne $IGNOREDIR3) {
			if ($thing =~ /imdb\.com\/Title\?(\d*)\"/) {$movie = $1; &getnfo; &trunct; &domessg};
		} else {print "Wont post $pathtorel. Its in a ignored area\n"};
	};
};
sub getnfo {	
	$url = "http://us.imdb.com/Title?$movie";
	$data1 = get "http://us.imdb.com/business?$movie";
	$data2 = get "http://us.imdb.com/Title?$movie";
	### Pulling out shit
	# Title
	$data2 =~ /<TITLE>(.*)<\/TITLE>/;
	$title = $1;
	$title =~ s/\"//g;
	# Directed by
	$data2 =~ /Directed by<\/B><BR>\n <A HREF=\"\/Name\?(.*)\">/;
	$name = $1;
	$name =~ s/,\+/ /;
	($n1, $n2) = split(" ", $name);
	$name = "$n2 $n1";
	# Genre
	$data2 =~ /\/Sections\/Genres\/(\w*)\//g;
	if ($2 ne "") {$genre = "$1 / $2"} else {$genre = $1};
	# Plot
	$data2 =~ /<B CLASS=\"ch\">Plot Outline: <\/B>(.*) <A HREF=\"\/Plot\?/;
	if ($1 eq $genre) {$plot = "Unavailable..."} else {$plot = $1};
	# User Rating
	$data2 =~ /<\/A> <B>(.*)<\/B>\/10 \((\d*)/;
	if ($1 ne $plot and $1 ne $genre) {$rating = "$1 / 10"; $rrating = $1} else {$rating = "N/A"};
	# Opening weekend
        $data1 =~ /<!-- OW --><\/B><\/DT>\n  <DD><DIV CLASS=botpad>(.*) \((.*)\) \(<A HREF=\"(.*)\">(\d*) (\w*)<\/A> <A HREF=\"(.*)\">(\d*)<\/A>\) \((.*)\)<B/;
        $tickets = $1;
	if ($tickets ne $genre and $tickets ne $plot and $tickets ne $rrating) {
		$tickets =~ s/\&pound\;/£/;
		$business = "business: $title played in $tickets on $8 on its opening weekend the $4 $5 / $7 in $2";
        } else {
		$data1 =~ /<B>Budget<!-- BT --><\/B><\/DT>\n  <DD><DIV CLASS=botpad>(.*)<BR>\n/;	
		$budget = $1;
		$data1 =~ /<B>Filming Studio Location<!-- ST --><\/B><\/DT>\n  <DD><DIV CLASS=botpad>(.*)<\/DIV><\/DD>\n/;
		$location = $1;
		if ($budget ne $genre and $budget ne $plot and $budget ne $rrating) {$business = "business: $title had a budget of $budget and was filmed in $location"}
		else {$business = "business: Sorry, no business results available"};
	};
	$LOGFORMAT =~ s/\?time/$time/;
        $LOGFORMAT =~ s/\?title/$title/;
        $LOGFORMAT =~ s/\?url/$url/;
        $LOGFORMAT =~ s/\?plot/$plot/;
        $LOGFORMAT =~ s/\?genre/$genre/;
        $LOGFORMAT =~ s/\?name/$name/;
        $LOGFORMAT =~ s/\?rating/$rating/;
	$LOGFORMAT =~ s/\?business/$business/;
        open GLFTPDLOG, ">> $PATH_TO_GLFTPD_LOG" or die "Couldnt open GLFTPD.LOG";
        print GLFTPDLOG "$LOGFORMAT\n";
        close GLFTPDLOG;
	$LOGFORMAT = $TmP4;
};
sub domessg {
	if ($APPENDIMDB == 1) {
	$pathtorel =~ s/ $//;
	$messagefile = $pathtorel."/.message";
	open MESSAGE, ">> $messagefile" or die "Couldnt open $messagefile\n"; 
	open TEMPLATE, "< $TEMPLATE" or die "Couldnt open your template file!";
	@MESSAGE = <TEMPLATE>;
	close TEMPLATE;
	foreach (@MESSAGE) { s/\?title/$title/g; };
	foreach (@MESSAGE) { s/\?url/$url/g; };
	foreach (@MESSAGE) { s/\?plot/$plot/g; };
	foreach (@MESSAGE) { s/\?genre/$genre/g; };
	foreach (@MESSAGE) { s/\?name/$name/g; };
	foreach (@MESSAGE) { s/\?rating/$rating/g; };
	print MESSAGE "\n@MESSAGE";
	close MESSAGE;
	};
};
sub findnprint {
	$url = "http://us.imdb.com/Title?$ARGV[1]";
	$url2 = $url;
	$url2 =~ s/ /%20/g;
	$data1 = get "http://us.imdb.com/business?$ARGV[1]";
	$data2 = get "http://us.imdb.com/Title?$ARGV[1]";
	$data2 =~ /<TITLE>(.*)<\/TITLE>/;
	$test = $1;
	if ($test eq "IMDb title search") {
		$data2 =~ />Movies<\/A><\/H2>\n<OL><LI><A HREF=\"\/Title\?(\d*)\">/;
		$imdbnum = $1;
		if ($imdbnum ne "") {
			$data1 = get "http://us.imdb.com/business?$imdbnum";
			$data2 = get "http://us.imdb.com/Title?$imdbnum";

	                ### Pulling out shit
	                # Title
        	        $data2 =~ /<TITLE>(.*)<\/TITLE>/;
                	$title = $1;
             		$title =~ s/\"//g;
               		# Directed by
    		        $data2 =~ /Directed by<\/B><BR>\n <A HREF=\"\/Name\?(.*)\">/;
                	$name = $1;
                	$name =~ s/,\+/ /;
	                ($n1, $n2) = split(" ", $name);
	                $name = "$n2 $n1";
	                # Genre
	                $data2 =~ /\/Sections\/Genres\/(\w*)\//g;
	                if ($2 ne "") {$genre = "$1 / $2"} else {$genre = $1};
	                # Plot
        	        $data2 =~ /<B CLASS=\"ch\">Plot Outline: <\/B>(.*) <A HREF=\"\/Plot\?/;
	                if ($1 eq $genre) {$plot = "Unavailable..."} else {$plot = $1};
	                # User Rating
        	        $data2 =~ /<\/A> <B>(.*)<\/B>\/10 \((\d*)/;
	                # Opening weekend
	        	$data1 =~ /<!-- OW --><\/B><\/DT>\n  <DD><DIV CLASS=botpad>(.*) \((.*)\) \(<A HREF=\"(.*)\">(\d*) (\w*)<\/A> <A HREF=\"(.*)\">(\d*)<\/A>\) \((.*)\)<B/;
	                $tickets = $1;
	                if ($tickets ne $genre and $tickets ne $plot and $tickets ne $rrating) {
                        $tickets =~ s/\&pound\;/£/;
                        $business = "business: $title played in $tickets on $8 on its opening weekend the $4 $5 / $7 in $2";
	                } else {
                        $data1 =~ /<B>Budget<!-- BT --><\/B><\/DT>\n  <DD><DIV CLASS=botpad>(.*)<BR>\n/;
                        $budget = $1;
                        $data1 =~ /<B>Filming Studio Location<!-- ST --><\/B><\/DT>\n  <DD><DIV CLASS=botpad>(.*)<\/DIV><\/DD>\n/;
                        $location = $1;
                        if ($budget ne $genre and $budget ne $plot and $budget ne $rrating) {$business = "business: $title had a budget of $budget and was filmed in $location"}
                        else {$business = "business: Sorry, no business results available"};
        	        };
	                $TRIGGERFORMAT =~ s/\?title/$title/g;
	                $TRIGGERFORMAT =~ s/\?url/$url2/g;
        	        $TRIGGERFORMAT =~ s/\?plot/$plot/g;
	                $TRIGGERFORMAT =~ s/\?genre/$genre/g;
	                $TRIGGERFORMAT =~ s/\?name/$name/g;
	                $TRIGGERFORMAT =~ s/\?rating/$rating/g;
	                $TRIGGERFORMAT =~ s/\?business/$business/g;
	                print "$TRIGGERFORMAT\n";

		};
	};
	if ($test ne "Invalid title or title code" and $test ne "IMDb title search") {
		### Pulling out shit
		# Title
		$data2 =~ /<TITLE>(.*)<\/TITLE>/;
		$title = $1;
		$title =~ s/\"//g;
		# Directed by
		$data2 =~ /Directed by<\/B><BR>\n <A HREF=\"\/Name\?(.*)\">/;
		$name = $1;
		$name =~ s/,\+/ /;
		($n1, $n2) = split(" ", $name);
		$name = "$n2 $n1";
		# Genre
		$data2 =~ /\/Sections\/Genres\/(\w*)\//g;
		if ($2 ne "") {$genre = "$1 / $2"} else {$genre = $1};
		# Plot
		$data2 =~ /<B CLASS=\"ch\">Plot Outline: <\/B>(.*) <A HREF=\"\/Plot\?/;
		if ($1 eq $genre) {$plot = "Unavailable..."} else {$plot = $1};
		# User Rating
		$data2 =~ /<\/A> <B>(.*)<\/B>\/10 \((\d*)/;
		if ($1 ne $plot and $1 ne $genre) {$rating = "$1 / 10"; $rrating = $1} else {$rating = "N/A"};
		# Opening weekend
        	$data1 =~ /<!-- OW --><\/B><\/DT>\n  <DD><DIV CLASS=botpad>(.*) \((.*)\) \(<A HREF=\"(.*)\">(\d*) (\w*)<\/A> <A HREF=\"(.*)\">(\d*)<\/A>\) \((.*)\)<B/;
        	$tickets = $1;
		if ($tickets ne $genre and $tickets ne $plot and $tickets ne $rrating) {
			$tickets =~ s/\&pound\;/£/;
			$business = "business: $title played in $tickets on $8 on its opening weekend the $4 $5 / $7 in $2";
        	} else {
			$data1 =~ /<B>Budget<!-- BT --><\/B><\/DT>\n  <DD><DIV CLASS=botpad>(.*)<BR>\n/;	
			$budget = $1;
			$data1 =~ /<B>Filming Studio Location<!-- ST --><\/B><\/DT>\n  <DD><DIV CLASS=botpad>(.*)<\/DIV><\/DD>\n/;
			$location = $1;
			if ($budget ne $genre and $budget ne $plot and $budget ne $rrating) {$business = "business: $title had a budget of $budget and was filmed in $location"}
			else {$business = "business: Sorry, no business results available"};
		};
		$TRIGGERFORMAT =~ s/\?title/$title/g;
		$TRIGGERFORMAT =~ s/\?url/$url2/g;
		$TRIGGERFORMAT =~ s/\?plot/$plot/g;
		$TRIGGERFORMAT =~ s/\?genre/$genre/g;
		$TRIGGERFORMAT =~ s/\?name/$name/g;
		$TRIGGERFORMAT =~ s/\?rating/$rating/g;
		$TRIGGERFORMAT =~ s/\?business/$business/g;
		print "$TRIGGERFORMAT\n";
	} else {
		if ($test eq "Invalid title or title code") {print "Sorry, didnt find anything on $ARGV[1]\n"};
		if ($test eq "IMDb title search") {print "Found multiple hits on $ARGV[1], try be more specific\n"};
		if ($test ne "IMDb title search" and $test ne "Invalid title or title code") {print "Something went wrong. IMDB told me: $test\n"};
	}};
};
## Thats all folks, and remember, PHP SUCK

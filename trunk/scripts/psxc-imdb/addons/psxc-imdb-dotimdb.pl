#!/usr/bin/perl

# Just to show you that bash is not the real winner when it comes to
# parsing text, here's an example in perl. This will replace the text
# in .imdb with the formatted text below.
##########################################
#
# CONFIG
########

# Limit the number of genres/countries/languages/certifications/
# runtimes and names of actors here. Remember that you have similar
# variables in psxc-imdb.conf - and they override the ones here..
#
# To help you decide what to choose, here's my reccommended settings,
# in a condensed format. Depending on what format you wish to use,
# it can be a good thing to limit output.
# format 2 for instance have a reccommended value of 4 genres shown,
# and a maximium of 6 genres, etc.
# format 3 is chosen as the (current) standard.
# 
# format 1: 3/3, 1/1, 1/1, 1/1, 1/1, -/-
# format 2: 4/6, 2/3, 2/2, -/-, 1/2, 5/6
# format 3: 4/4, -/-, -/-, -/-, 1/1, 5/6
# format 4: 4/6, -/-, -/-, -/-, 1,4, 5,6
# format 5: 3/3, -/-, -/-, -/-, 1/1, 5/6
$NUMGENRE = 4;
$NUMCOUNTRY = 2;
$NUMLANGUAGE = 2;
$NUMCERTIFICATION = 1;
$NUMRUNTIME = 1;
$NUMCASTING = 5;

# IMPORTANT! #
##############
# Uncomment *ONE* of the following lines - the one you uncomment will be
# the one corresponding to the format you choose. More formats will come
# in future version(s).

#$myformat = 1;
#$myformat = 2;
$myformat = 3;
#$myformat = 4;
#$myformat = 5;

######################################################################
# Sort of END OF CONFIG - below is for experienced/experimental users
#
# Code follows - these are the different formats used in $myformat.
# You can edit these if you like, or make your own.
##############################################################################

sub format_one {
format DOTONE =
.---------------------------------|iMDB|-----------------------------------.
|                                                                          |
| @||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| |
  $IMDBTITLE
|                                                                          |
| @||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| |
  $IMDBURL
|--------------------------------------------------------------------------|
| genre...: ^<<<<<<<<<<<<<<<<<<<<<<<<<<... | ^<<<<<<<<<<<<<<<<<<<<<<<<<<<< |
            $IMDBGENRE                       $IMDBPLOT
| score...: [@<<<<<<<<<]  @<< @>>>>> votes | ^<<<<<<<<<<<<<<<<<<<<<<<<<<<< |
            $IMDBBAR,    $IMDBSCORE, $IMDBVOTES, $IMDBPLOT
| director: ^<<<<<<<<<<<<<<<<<<<<<<<<<<... | ^<<<<<<<<<<<<<<<<<<<<<<<<<<<< |
            $IMDBDIRECTOR,                   $IMDBPLOT
| rated...: ^<<<<<<... runtime.: ^<<<<<... | ^<<<<<<<<<<<<<<<<<<<<<<<<<<<< |
            $IMDBCERTIFICATION, $IMDBRUNTIME, $IMDBPLOT
| country.: ^<<<<<<... language: ^<<<<<... | ^<<<<<<<<<<<<<<<<<<<<<<<<<<<< |
            $IMDBCOUNTRY,    $IMDBLANGUAGE,  $IMDBPLOT
| starring: ^<<<<<<<<<<<<<<<<<<<<<<<<<<... | ^<<<<<<<<<<<<<<<<<<<<<<<<<... |
            $IMDBCASTLEADNAME,               $IMDBPLOT
|--------------------------------------------------------------------------|
| Number of screens: @<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< |
                     $IMDBNUMSCREENS
| Premiered in.....: @<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< |
                     $IMDBPREMIERE
`------------------------------------------------------------| 2003 psxc |-'
.
}

sub format_two {

format DOTTWO =
.---------------------------------|iMDB|-----------------------------------.
|                                                                          |
| @||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| |
  $IMDBTITLE
|                                                                          |
| @||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| |
  $IMDBGENRE
|--------------------------------------------------------------------------|
| score...: @<< (@>>>>> votes)        | runtime:.: ^<<<<<<<<<<<<<<<<<<<... |
            $IMDBSCORE, $IMDBVOTES,       $IMDBRUNTIME
| country.: ^<<<<<<<<<<<<<<<<<<<<<... | language.: ^<<<<<<<<<<<<<<<<<<<... |
            $IMDBCOUNTRY,                 $IMDBLANGUAGE
| premiere: ^<<<<<<<<<<<<<<<<<<<<<<<< | screens..: ^<<<<<<<<<<<<<<<<<<<... |
            $IMDBPREMIERE,                $IMDBNUMSCREENS
| director: ^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<... |
            $IMDBDIRECTOR
| cast....: ^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< |
            $IMDBCASTING
|           ^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<... |
            $IMDBCASTING
|--------------------------------------------------------------------------|
| tagline....: ^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< |
               $IMDBPLOT
|              ^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< |
               $IMDBPLOT
|              ^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<... |
               $IMDBPLOT
| usercomment: ^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< |
               $IMDBCOMMENTSHORT
|              ^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<... |
               $IMDBCOMMENTSHORT
`------------------------------------------------------------| 2003 psxc |-'
.
}

sub format_three {

$COMBO = $IMDBSCORE . " (" . $IMDBVOTES . " votes) - " . $IMDBRUNTIME . " - " . $IMDBGENRE;

format DOTTHREE =
.---------------------------------|iMDB|-----------------------------------.
|                                                                          |
| @||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| |
  $IMDBTITLE
| @||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| |
  $COMBO
|--------------------------------------------------------------------------|
| imdb link..: @<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< |
               $IMDBURL
| cast.......: ^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< |
               $IMDBCASTING
|              ^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<... |
               $IMDBCASTING
| usercomment: ^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< |
               $IMDBCOMMENTSHORT
|              ^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<... |
               $IMDBCOMMENTSHORT
`------------------------------------------------------------| 2003 psxc |-'
.
}

sub format_four {

$IMDBVOTESMOD = "(" . $IMDBVOTES . " votes)";
$IMDBURLMOD = "[ " . $IMDBURL . " ]";

format DOTFOUR =
ÛßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßÛ
Û °°°°°±±±±±²²²²²²ÛÛÛÛ    ÜÜÜÜÜÜÜ ÜÜÜ ÜÜÜ ÜÜÜÜÜÜ  ÜÜÜÜÜÜÜ    ÛÛÛÛ²²²²²²±±±±±°°°°° Û
Û °°°°°±±±±±²²²²²²²       ÛÜÜ ÜÜÛ Û ßÛß Û Û ÜÜÜ Û Û ÜÜÜ Û       ²²²²²²²±±±±±°°°°° Û
Û °°°°°±±±±±²²²²²²²       ÜÜÛ ÛÜÜ Û Ü Ü Û Û ÛÛÛ Û Û ÜÜÜßÛ       ²²²²²²²±±±±±°°°°° Û
Û °°°°°±±±±±²²²²²²²       ÛÜÜÜÜÜÛ ÛÜÛÛÛÜÛ ÛÜÜÜÜÜß ÛÜÜÜÜÜÛ  (c)  ²²²²²²²±±±±±°°°°° Û
Û °°°°°± ÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜ ±°°°°° Û
Û °°°°°± Û                                                               Û ±°°°°° Û
Û °°°°°± Û   @|||||||||||||||||||||||||||||||||||||||||||||||||||||||||  Û ±°°°°° Û
             $IMDBTITLE
Û °°°°°± Û                                                               Û ±°°°°° Û
Û °°°°°± Û   @|||||||||||||||||||||||||||||||||||||||||||||||||||||||||  Û ±°°°°° Û
             $IMDBURLMOD
Û °°°°°± Û                                                               Û ±°°°°° Û
Û °°°°°± Û   Score........: [@<<<<<<<<<]  @<<  @<<<<<<<<<<<<<<<<<<<<<<<  Û ±°°°°° Û
                             $IMDBBAR,    $IMDBSCORE, $IMDBVOTESMOD
Û °°°°°± Û   Genres.......: @<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<...  Û ±°°°°° Û
                            $IMDBGENRE
Û °°°°°± Û   Runtime......: @<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<...  Û ±°°°°° Û
                            $IMDBRUNTIME
Û °°°°°± Û   Directed by..: @<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<...  Û ±°°°°° Û
                            $IMDBDIRECTOR
Û °°°°°± Û   Cast.........: ^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  Û ±°°°°° Û
                            $IMDBCASTING
Û °°°°°± Û                  ^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<...  Û ±°°°°° Û
                            $IMDBCASTING
Û °°°°°± Û                                                               Û ±°°°°° Û
Û °°°°°± Û   Screens......: @<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<...  Û ±°°°°° Û
                            $IMDBNUMSCREENS
Û °°°°°± Û   Businessdata.: @<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<...  Û ±°°°°° Û
                            $IMDBBUSINESSDATA
Û °°°°°± Û   Premiere info: @<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<...  Û ±°°°°° Û
                            $IMDBPREMIERE
Û °°°°°± Û                                                               Û ±°°°°° Û
Û °°°°°± Û   Tagline......: ^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  Û ±°°°°° Û
                            $IMDBTAGLINE
Û °°°°°± Û                  ^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<...  Û ±°°°°° Û
                            $IMDBTAGLINE
Û °°°°°± Û   User comment.: ^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  Û ±°°°°° Û
                            $IMDBCOMMENTSHORT
Û °°°°°± Û                  ^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  Û ±°°°°° Û
                            $IMDBCOMMENTSHORT
Û °°°°°± Û                  ^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<...  Û ±°°°°° Û
                            $IMDBCOMMENTSHORT
Û °°°°°± Û                                                               Û ±°°°°° Û
Û °°°°°± ßßßßßßßßßßßßßßÛßßßßßÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛßßßßßÛßßßßßßßßßßßßßß ±°°°°° Û
Û °°°°°±±±±±²²²²²²²    ÛÛ        (psxc imdb 2003)         ÛÛ    ²²²²²²²±±±±±°°°°° Û
Û °°°°°±±±±±²²²²²²²    ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß    ²²²²²²²±ruzz 2k3° Û
ÛÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÛ
.
}

sub format_five {

$COMBO = "[ " . $IMDBGENRE . " ]-[ " . $IMDBSCORE . " - " . $IMDBVOTES . " votes ]-[ " . $IMDBRUNTIME . " ]-[ " . $IMDBCERTIFICATION . " ]";

format DOTFIVE =
 ÉÍÍÍÍÍÍÍ»                                                               ÉÍÍÍÍÍÍÍ»
É¼ÉÍÍÍÍÍÍÊÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÊÍÍÍÍÍÍ»È»
ÈÎ¼  @||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||  ÈÎ¼
     $IMDBTITLE
 ÈÍÍÍÍÍÍÍËÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍËÍÍÍÍÍÍÍ¼
ÉÍÍÍÍÍÍÍÍÊÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÊÍÍÍÍÍÍÍÍ»
º  @||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||  º
   $COMBO
ÌÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¹
º  Director: @<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<...  º
             $IMDBDIRECTOR
º  Cast....: ^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  º
             $IMDBCASTING
º            ^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<...  º
             $IMDBCASTING
º  Plot....: ^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  º
             $IMDBPLOT
º            ^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  º
             $IMDBPLOT
º            ^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<...  º
             $IMDBPLOT
ÌÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄËÄÄËÄÄÄËÄÄÄÄËÍÄÄÄÍËÍÄÄÄÍËÄÄÄÄËÄÄÄËÄÄËÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¹
º psxc imdb 2003   ÉÍ» º  º   º    º     º     º    º   º  º ÉÍ»         ruzz 2k3 º
ÈËÄÍÍÍÍÍÄÍÍÍÍÍÍÍÍÍÍºÍ¼ÍÊÍÍÊÍÍÍÊÍÍÍÍÊÍÍÍÍÍÊÍÍÍÍÍÊÍÍÍÍÊÍÍÍÊÍÍÊÍÈÍºÍÍÍÍÍÍÍÍÍÍÄÍÍÍÍÍÄË¼
É¼ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÊÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÊÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ»È»
ÈÎ¼  @||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| ÈÎ¼
     $IMDBURL
 ÈÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼
.
}

# No need to edit below #
#########################

use Text::ParseWords;

sub trim {
        my @out = @_;
        for (@out) {
                s/^\s+//;
                s/\s+$//;
        }
        return wantarray ? @out : $out[0];
}

sub limit {
        my @in = @_;
        my $in = $in[0];
        my $count = $in[1];
	my $sep = $in[2];
        my @out = &quotewords("$sep", 0, $in);
        @in = trim(@out);
        my $out = "$in[0]";
	if ($count > 1 && $#in > 0 ) {
	        for ($i = 1; $i < $count && $i <= $#in; $i++) {
	                $out = $out . ", " . "$in[$i]";
	        }
	}
	return $out;
}

# First, let's grab the variables passed and assign names. A bit easier
# to do in perl than in a shell script ;)
@imdbvars = &quotewords('\s+', 0, @ARGV);
@imdbvars = trim(@imdbvars);
($IMDBDATE, $IMDBDOTFILE, $IMDBRELPATH, $IMDBDIRNAME, $IMDBURL, $IMDBTITLE,
 $IMDBGENRE, $IMDBRATING, $IMDBCOUNTRY, $IMDBLANGUAGE, $IMDBCERTIFICATION,
 $IMDBRUNTIME, $IMDBDIRECTOR, $IMDBBUSINESSDATA, $IMDBPREMIERE, $IMDBLIMITED,
 $IMDBVOTES, $IMDBSCORE, $IMDBNAME, $IMDBYEAR, $IMDBNUMSCREENS,
 $IMDBISLIMITED, $IMDBCASTLEADNAME, $IMDBCASTLEADCHAR, $IMDBTAGLINE,
 $IMDBPLOT, $IMDBBAR, $IMDBCASTING, $IMDBCOMMENTSHORT, 
 $IMDBCOMMENTLONG) = @imdbvars;

# limit stuff
$IMDBGENRE = limit($IMDBGENRE, $NUMGENRE, "/");
$IMDBCOUNTRY = limit($IMDBCOUNTRY, $NUMCOUNTRY, "/");
$IMDBLANGUAGE = limit($IMDBLANGUAGE, $NUMLANGUAGE, "/");
$IMDBCERTIFICATION = limit($IMDBCERTIFICATION, $NUMCERTIFICATION, "/");
$IMDBRUNTIME = limit($IMDBRUNTIME, $NUMRUNTIME, "/");
$IMDBCASTING = limit($IMDBCASTING, $NUMCASTING, ",");

# choose the format.
if ( $myformat == 1 ) { open(DOTONE, ">$IMDBDOTFILE"); format_one; write DOTONE; close(DOTONE);
} elsif ( $myformat == 2 ) { open(DOTTWO, ">$IMDBDOTFILE"); format_two; write DOTTWO; close(DOTTWO);
} elsif ( $myformat == 3 ) { open(DOTTHREE, ">$IMDBDOTFILE"); format_three; write DOTTHREE; close(DOTTHREE);
} elsif ( $myformat == 4 ) { open(DOTFOUR, ">$IMDBDOTFILE"); format_four; write DOTFOUR; close(DOTFOUR);
} elsif ( $myformat == 5 ) { open(DOTFIVE, ">$IMDBDOTFILE"); format_five; write DOTFIVE; close(DOTFIVE);
}
exit 0


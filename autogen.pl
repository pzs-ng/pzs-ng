#!/usr/bin/perl -w

# search for usable auto* toolset

@autoconf_versions = ("2.59", "2.57", "2.5");

$use_autoconf = 0;

sub testversion {
  $version = shift;

  for $v (@_) {
    if ($version =~ /$v/) {
      return 1;
    }
  }
  return 0;
}

$paths = $ENV{'PATH'};
@paths = split(":", $paths);

@autoconf = ();

for $p (@paths) {
  @autoconf = (@autoconf, glob("$p/autoconf*"));
}

print "Testing autoconf:\n";
for $a (@autoconf) {
  open (VERSION, "$a --version |") || next;
  $v = <VERSION>;
  close (VERSION);
  $v =~ /.* (\S+)/;
  $usable = testversion($1, @autoconf_versions);
  print " $a " . ("[OLD]", "[OK]")[$usable] . "\n";
  $use_autoconf = $a if (!$use_autoconf && $usable);
}

if ($use_autoconf) {

  print "Running autoconf ...\n";
  system($use_autoconf);

  print "Complete!\n";
} else {
  print "Error: Couln'd find required auto* tools!\n";
}

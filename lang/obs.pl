#!/usr/bin/perl
#
# Checks a language file against the reference language (en_us), and
# finds #define string diferences
#
my $srcfile = $ARGV[0];
my $engfile = "en_us.l";
my %is_there= ();
my $pos=0;
my $wc=0;
my $edef="";
my $sdef="";

my @efile;
my @sfile;
my @defs;

sub error {

    my $msg = shift ;
    print "*** Error: $msg \n";
    exit ;
}

sub not_del {
    $line = shift;
    if ($is_there{$line}) {
        return 1;
    } else {
        return 0;
    }

}

sub get_format {
    my $fmt="";

    my $str=shift;

    while ($str =~ m/%\w/g) {
        $fmt .= $&;
    }
   
    return $fmt;
}

# Could not think of a worse of doing this...
sub get_def {
    my $ndef;
    my $start=0;
    my $found=0;

    $buf = shift;
    $def = shift;
    for (@$buf) {
        my $line = $_;
        chomp $line;

        if (/^[A-Z]/) {
            $start=0;
            last if $found;
        }

        if ($start) {
            $found=1;
            $ndef.=$_;
        }

        if ($line eq $def) {
            $start=1;
        }

    }
    return $ndef;
}

error("Usage $0 [lang.l]") unless ($srcfile);
error("Can't find language file: $srcfile") if (! -f $srcfile);
error("Can't find language file: $engfile") if (! -f $engfile);

open (EFILE, "< $engfile");
while (<EFILE>) {
    my $line = $_;
    push(@efile, $line);
    chomp $line;
    if (/^[A-Z]/) {
        $wc = push (@defs, $line);
        $is_there{$line} = 1;
    }
}
close(EFILE);

open (SFILE, "< $srcfile");
while (<SFILE>) {
    push(@sfile, $_);
}
close(SFILE);

#for (@defs) { 
#    $is_there{$_} = 1;
#}

for (@sfile) {
    my $line = $_;
    chomp $line;
    if (/^[A-Z]/) {
        if (not_del($line)) {
            while ($line ne $defs[$pos]) {
                print "ADD: $defs[$pos]\n";
                $pos++;
            }
            if (! /^STRFTIME/ ) {
                $edef = get_format(get_def(\@efile, $line)) ;
                $sdef = get_format(get_def(\@sfile, $line)) ;
                if ( $edef ne $sdef ) {
                   print "FORMAT: $line (expecting '$edef' and got '$sdef')\n";
                }
            }
            $pos++;
        } else {
            print "DEL: $line\n";
        }
    }
}

for($pos ; $pos < $wc; $pos++) {
    print "ADD: $defs[$pos]\n";
}


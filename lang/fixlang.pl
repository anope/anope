#!/usr/bin/perl
#
# Tries to fix a language file using the reference 
# language (en_us), and injecting all new strings
# (in english) into the newly generated fixed.pl
#
# check the result with obs.pl then copy on top
# of the source language file.
#
my $srcfile = $ARGV[0];
my $dstfile = "fixed.l";
my $engfile = "en_us.l";
my %is_there= ();
my $pos=0;
my $wc=0;

my @defs;
my @efile;

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

# Could not think of a worse of doing this...
sub new_def {
    my $ndef;
    my $start=0;

    $def = shift;
    for (@efile) { 
        my $line = $_;
        chomp $line;

        if (/^[A-Z]/) {
            $start=0;
        }

        if ($start) {
            $ndef.=$_;
        }

        if ($line eq $def) {
            $ndef=$_;
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
    }
}
close(EFILE);

# Need to load entiry text into hash...
for (@defs) { 
    $is_there{$_} = 1;
}

open (SFILE, "< $srcfile");
open (DFILE, "> $dstfile");
while (<SFILE>) {
    my $line = $_;
    chomp $line;
    if (/^[A-Z]/) {

        $del = 0;
        if (not_del($line)) {
            while ($line ne $defs[$pos]) {
                print "ADD: $defs[$pos]\n";
                # Need to reference the hash...
                my $ndef = new_def($defs[$pos]);
                print DFILE $ndef;
                $pos++;
            }
            $pos++;
        } else {
            $del = 1;
            print "DEL: $line\n";
        }
    }

    if (! $del) {
        print DFILE "$line\n";
    }
}

for($pos ; $pos < $wc; $pos++) {
    print "ADD: $defs[$pos]\n";
    my $ndef = new_def($defs[$pos]);
    print DFILE $ndef;
}
close(SFILE);
close(DFILE);

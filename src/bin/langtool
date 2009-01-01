#!/usr/bin/perl
#
# Ribosome's all in one language tool
# 1) Compares to check for missing lines
# 2) Compares to check for equal lines
# 3) Fixes (on request) missing lines
# 4) Checks for errors in lines
# 5) Checks for long lines (>60 characters)
#

# Check to see if we have received arguments
if (!$ARGV[0]) {
print "Usage: $0 [language file]\n";
exit;
}

# If yes, set $sourcefile to the language filename
$testagainst = $ARGV[0];

# Set $sourcefile to en_us language file
$sourcefile = "en_us.l";

# Number of Keys in File, Number of Equal Lines, Number of Missing Lines
# Fixed Lines (optional) and Number of Format Error Lines, Number of Long Lines
my $numberlines = 0;
my $equallines = 0;
my $missinglines = 0;
my $fixedlines = 0;
my $errorline = 0;
my $longlines = 0;

# Array to hold the lines from the source file and explosion array for long line check

my @sourcearray;
my @explosion;

# Create associative arrays which will contain keys and definitions
# One for the control source and one for what is being tested against

my %sourcehash= ();
my %testhash= ();

# Check if Files Exist

if (! -f $sourcefile) {
print "Critical Error: The source file does not exist or is unreadable.\nNote: This tool must be run from the language directory.\n";
exit;
}

if (! -f $testagainst) {
print "Critical Error: The language file does not exist or is unreadable\n";
exit;
}
###########################################
# Function to load sourcefile into memory #
###########################################

sub load_sourcefile {

    my $_key; # This variable will hold the key
    my $_def; # This variable will hold the definition
    my $line; # This variable will hold the current line

    open (SOURCEFILE, "< $sourcefile"); # Open the source file
    while (<SOURCEFILE>) { 		# For each line the source file
        $line = $_; 			# $line is set to the line in the source file

        # Skip comments
        if (/^#/) {	# This checks for # which indicates comment
	next;		# If detected, the next line is skipped to
        }

        # Start of a key
        if (/^[A-Z]/) {				# Checks to see if the line is a key
            chomp($line);			# Removes things that shouldn't be at the end
            push (@sourcearray, $line);		# Puts the line into the source array
						# If a key is defined, load definition into the hash
            if ($_key ne undef) {		# If the key is defined
                $sourcehash{$_key} = $_def;	# Add the definition to the associative array
            }
            $_key=$line;		# Set the key to the line
            $_def="";			# Reset the definition
            next;			# Move onto the next line
        }

        if ($_key ne undef) {		# If the key is set already
            $_def.=$line;		# The definition is the current line
        }

	$sourcehash{$_key} = $_def;	# Load the definition into the associative array
	} 				# End of while which reads lines

	close (SOURCEFILE);		# Close the source file

}					# End of load_sourcefile function

#################################################
# Function to load testagainst file into memory #
#################################################

sub load_testagainst {

    my $_key; # This variable will hold the key
    my $_def; # This variable will hold the definition
    my $line; # This variable will hold the current line

    open (TESTAGAINSTFILE, "< $testagainst");	# Open the file containing the control lines
    while (<TESTAGAINSTFILE>) {			# For each line in the file
        $line = $_;				# $line is set to the lines value


        # Skip comments
        if (/^#/) {     # This checks for # which indicates comment
        next;           # If detected, the next line is skipped to
        }

        # Start of a key
        if (/^[A-Z]/) {                         # Checks to see if the line is a key
            chomp($line);      			# Removes things that shouldn't be at the end


            if ($_key ne undef) {               # If the key is defined
                $testhash{$_key} = $_def;     # Add the definition to the associative array
            }
            $_key=$line;                # Set the key to the line
            $_def="";                   # Reset the definition
            next;                       # Move onto the next line
        }

        if ($_key ne undef) {           # If the key is set already
            $_def.=$line;               # The definition is the current line
        }

        $testhash{$_key} = $_def;	# Load the definition into the associative array
        }                               # End of while which reads lines
        close (TESTAGAINSTFILE);	# Close the source file

}


sub get_format {	# Function to get the formatting from a string
    my $fmt="";
    my $str=shift;	# Get the input

    while ($str =~ m/%\w/g) {
        $fmt .= $&;
    }
   
    return $fmt;	# Return the formatting
}


load_sourcefile;	# Call function to load the source file
load_testagainst;	# Call function to load the test file

if (!grep(/^LANG_NAME/,%testhash)) {
print "Critical Error: $ARGV[0] is not a valid language file!\n";
exit;
}

open (LOG,"> langcheck.log");	# Open logfile for writing

foreach $sourcearray (@sourcearray) {	# For each key stored in the source array
    my $_key=$_;			# Store key from source array in $_key
    
	if ((get_format($sourcehash{$sourcearray})) ne (get_format($testhash{$sourcearray}))) {
	if ($sourcearray !~ STRFTIME )  {
	$errorline++;
	print LOG "FORMAT: $sourcearray - (expecting '".get_format($sourcehash{$sourcearray})."' and got '".get_format($testhash{$sourcearray})."')\n";
	} }

	if ($testhash{$sourcearray} eq $sourcehash{$sourcearray} ) {
							# If the definitions between the source and the test are equal
	$equallines++; 					# Number of equal lines increases
	print LOG "EQUAL: $sourcearray\n";		# Line is written to logfile
	}

	if (!grep(/^$sourcearray/,%testhash)) { 		# If the key is found in the testhash associative array
	$missinglines++; 					# Increase missing lines
	print LOG "MISSING: $sourcearray\n";			# Line is written to logfile
	}

        @explosion = split(/\n/, $testhash{$sourcearray});
        foreach $explosion (@explosion) {
        $explosion =~ s/\002//g;
        $explosion =~ s/\037//g;
        if (length($explosion) > 61) {
        print LOG "LONGLINE: $sourcearray (".substr($explosion,1,30)."...)\n";
        $longlines++;
        }
        }
	
$numberlines++;							# Increase the number of lines tested by 1
}
close (LOG);							# Close the logfile


#########################
# Show the test results #
#########################

print "Calculation Results:\n";
print "----------------------------------\n";
print "[$numberlines] line(s) were compared\n";
print "[$equallines] line(s) were found to equal their $sourcefile counterpart(s)\n";
print "[$missinglines] line(s) were found to be missing\n";
print "[$longlines] line(s) were found to be long (>60 chars)\n";
print "[$errorline] line(s) were found to have formatting errors\n";
print "The specific details of the test have been saved in langcheck.log\n";

if ($missinglines) {					# If missing lines exist
print "----------------------------------\n";
print "Missing line(s) have been detected in this test, would you like to fix the file?\n";
print "This automatically inserts values from the control file ($sourcefile) for the missing values\n";
print "y/N? (Default: No) ";

my $input = <STDIN>;							# Ask if the file should be fixed
if ((substr($input,0,1) eq "y") || (substr($input,0,1) eq "Y")) {	# If Yes...

open (FIX, ">> $testagainst");		# Open the file and append changes

foreach $sourcearray (@sourcearray) {   # For each key stored in the source array
    my $_key=$_;                        # Store key from source array in $_key

        if (!grep(/^$sourcearray/,%testhash)) {                 # If the key is not found in the testhash associative array
	print FIX "$sourcearray\n$sourcehash{$sourcearray}";	# Add the line(s) to the language file
	$fixedlines++;						# Increase the fixed line count
        }
}

close (FIX);							# Close file after fixing

print "Fixing Compete. [$fixedlines] line(s) fixed.\n";
exit;								# And Exit!
}
								# Otherwise, quit without fixing
print "Exiting. The language file has NOT been fixed.\n";

}

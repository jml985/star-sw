#!/usr/bin/perl -w
# doFlowSubmit.pl is a perl script to submit doFlowEvents.C for each centrality
# The $homeDir directory should contain a link to your inDir, and to your outDir
# The $macDir directory should contain doFlowEvents.C
# The $rootDir directory should contain the library dot file
# $inDir should be a link to a directory containing subdirectories of files
# $baseRunNo must have two digits ending in zero
# $outDir is a link to where you want your output
# It runs the second pass in the same subdirectory as the first pass
# If the 2nd pass has completed there is no submission
# If multiple centralities are selected, the first pass will be firstPass="kTRUE"
# On the 2nd pass these files in dir 0 will be copied to dirs 1-9
# To run again using the same flowPhiWgt.hist.root files, do
#  rm outDir/*/flowPhiWgtNew.hist.root
# To run again using the same flowReCentAna.hist.root files, do
#  rm outDir/*/flowReCentAnaNew.hist.root
# To run again using the same flow.firstPassLYZ.root files, do
#  rm outDir/*/flow.firstPassLYZNew.root
# In all cases rm outDir/*/doFlowEvents.C if it has changed
# by Kirill Filimonov and Art Poskanzer

use strict;
use File::Basename;
use File::Copy;
use Getopt::Std;

my $events = 30000;
#my $events = 1000;
my $queTime = "24:00:00";
#my $queTime = "01:00:00";
my $reCent = "kTRUE"; # must be the same as in doFlowEvents.C

# For centralities 1 to 9
my $nJobs = 9;
my $firstJob = 1;

# For only centrality 0, baseRunNo must end in zero
#my $nJobs = 1;
#my $firstJob = 0;

# options
# -t :test, no submission
my %opt;
getopts('t',\%opt);

my $usage = "usage: -opts  inDir baseRunNo [outPicoDir]\n";

# set directories
my $inDir = shift or die $usage;
(my $defaultOut = $inDir) =~ s/\.dst/\.mdst/;
my $outDir = "./outDir"; # directory for output
my $baseRunNo = shift or die $usage;
my $outPicoDir = shift || $defaultOut;
-e $inDir or die "$inDir :$!\n";
-e $outPicoDir or die "$outPicoDir :$!\n";

###### User defined:
my $homeDir = "~posk/root"; # directory containing the links
my $macDir = "~posk/root/mac"; # directory containing the macro
my $rootDir = "~posk/root"; # directory containing the library
print "homeDir = $homeDir\n";
print "inDir = $inDir\n";
print "outDir = $outDir\n";
print "macDir = $macDir\n";
print "rootDir = $rootDir\n";
if ($outPicoDir ne $inDir) {
  print "outPicoDir = $outPicoDir\n";
}
#my $fileExt = "*.flowpicoevent.root";
my $fileExt = "*MuDst.root";

my $starVer = basename $ENV{STAR};
my $io = 1;
my $resource = '';

# which machine are we at?
if ($ENV{HOST} =~ /pdsf/) {
  if ($inDir eq "in4") {
    $resource = "-l eliza9io=$io"; # in4
  } elsif ($inDir eq "in7") {
    $resource = "-l eliza12io=$io"; # in7
  } else {
    print "no io resource\n";
  }
} else {
  die "wrong host $ENV{HOST}\n";
}

print "host = $ENV{HOST}\n";
print "resource = $resource\n";
print "star ver.= $starVer\n";
print "max time = $queTime\n";
print "events = $events\n";
print "reCent = $reCent\n";

my $command;
my $job;
my $lastJob;
my $firstPass;
my $firstPassDir;
my $runNo;
my $workDir;
my $exec = "qsub -V -m e"; # for mail at the end
#my $exec = "qsub -V"; # for no mail
my $subDirNo;

# subdirectory loop
my @subDirs;
if ( `ls -l $inDir/ | grep ^d` ) { # subdirectories?
  @subDirs = split(/\n/,`ls $inDir`); # yes
} else {
  @subDirs = "."; # no subdirectories
}
print "subdirectories: @subDirs\n";
foreach my $subDir (@subDirs) {
  if ( -d "$inDir/$subDir" && `ls $inDir/$subDir/` ) { # dir exists and contains a file
    $job = $firstJob;
    $lastJob = $firstJob + $nJobs;
    $firstPass = "kFALSE";
    $firstPassDir = "$outDir/$inDir-$subDir-$baseRunNo";
    $subDirNo = substr($subDir, index($subDir, ".")+1, 4); # 4 chars after the dot
# uncomment the next if running ONLY the AnalysisMaker:
    if ($firstJob) { # first job not zero
      (-e "$firstPassDir/flowPhiEgt9.root" || -e "$firstPassDir/flowReCent9.root") or do {
	$job = 0;
	$lastJob= 1;
	$firstPass = "kTRUE";  # make firstPass files
      };
    }

    # centrality loop
    while ($job < $lastJob) {
      $runNo = $baseRunNo + $job;
      # make work directory
      $workDir = "$outDir/$inDir-$subDir-$runNo";
      -e $workDir or do {
	print "making $workDir directory\n";
	mkdir $workDir,0755 or die "$!\n";
      };
      # copy doFlowEvents.C
      -e "$workDir/doFlowEvents.C" or do {
	$command = "cp $macDir/doFlowEvents.C $workDir/doFlowEvents.C"; # don't use copy
	print $command,"\n";
	`$command`;
#	if (copy("$macDir/doFlowEvents.C","$workDir/")) { # does not work
#	  print "$macDir/doFlowEvents.C copied to $workDir/doFlowEvents.C\n";
#	}
      };
      # make links
      -e "$workDir/.sl44_gcc346" or do {
	$command = "ln -s $rootDir/.sl44_gcc346 $workDir/.sl44_gcc346";
	print $command,"\n";
	`$command`;
      };
      #my $pidFile="$homeDir/PIDTable.root";
      #if (-e "$pidFile") {
	#-e "$workDir/PIDTable.root" or do {
	  #$command = "ln -s $pidFile $workDir/PIDTable.root";
	  #print $command,"\n";
	  #`$command`;
	#};
      #}
      -e "$workDir/$inDir" or do {
	$command = "ln -s $homeDir/$inDir $workDir/$inDir";
	print $command,"\n";
	`$command`;
      };
      if ($outPicoDir ne $inDir) {
	-e "$workDir/$outPicoDir" or do {
	  $command = "ln -s $homeDir/$outPicoDir $workDir/$outPicoDir";
	  print $command,"\n";
	  `$command`;
	};
      }

      my $flowReCentX = "$firstPassDir/flowReCent$job.root";
      my $flowReCent = "$workDir/flow.reCentAna.root";
      my $flowReCentNew = "$workDir/flow.reCentAnaNew.root";
      my $flowPhiWgtX = "$firstPassDir/flowPhiWgt$job.root";
      my $flowPhiWgt = "$workDir/flowPhiWgt.hist.root";
      my $flowPhiWgtNew = "$workDir/flowPhiWgtNew.hist.root";
      if ($reCent eq "kTRUE") {
	if (-e $flowReCentX) {
	  -e $flowReCent or do { # copy it
	    if (copy("$flowReCentX","$flowReCent")) {
	      print "$flowReCentX copied to $flowReCent\n";
	    }
	  };
	}
	-e $flowReCent or do { # rename new
	  if (rename "$flowReCentNew", "$flowReCent") {
	    print "$flowReCentNew moved to $flowReCent\n";
	  }
	};
      } else { # phiWgt
	if (-e $flowPhiWgtX) {
	  -e $flowPhiWgt or do { # copy it
	    if (copy("$flowPhiWgtX","$flowPhiWgt")) {
	      print "flowPhiWgt$job.root copied to flowPhiWgt.hist.root\n";
	    }
	  };
	}
	-e $flowPhiWgt or do { # rename new
	  if (rename "$flowPhiWgtNew", "$flowPhiWgt") {
	    print "flowPhiWgtNew.hist.root moved to flowPhiWgt.hist.root\n";
	  }
	};
      }

      my $flowFirstPassLYZ = "$workDir/flow.firstPassLYZ.root";
      my $flowFirstPassLYZNew = "$workDir/flow.firstPassLYZNew.root";
      -e $flowFirstPassLYZ or do {
	if (rename "$flowFirstPassLYZNew", "$flowFirstPassLYZ") {
	  print "flow.firstPassLYZNew.root moved to flow.firstPassLYZ.root\n";
	}
      };

      my $secondPassDone = "kFALSE";
      if (-e $flowPhiWgtNew || -e $flowFirstPassLYZNew || -e $flowReCentNew) {
	$secondPassDone = "kTRUE";
	print "$subDir-$runNo secondPassDone\n";
      } # Dont know why this does not compile

      # make the shell script
      my $scriptName = "$workDir/flow.csh";
      open(FH,">$scriptName") or die "$scriptName : $!\n";
      print FH <<EOF;
#!/bin/csh
#\$ -o /dev/null
#\$ -e /dev/null
#\$ -M AMPoskanzer\@LBL.gov
#\$ -m a
cd $workDir
source \$GROUP_DIR/.starver $starVer
set logDir = \$SCRATCH
set log = \$logDir/ana.log
root4star -b << FINIS >&! \$log
Int_t RunType = $runNo;
const Char_t* OutPicoDir = "./$outPicoDir/";
.L doFlowEvents.C
doFlowEvents($events,"$inDir/$subDir","$fileExt",$firstPass)
.q
FINIS
set outLog = ana.log
head -130 \$log > \$outLog
echo " " >> \$outLog
echo "########################  head -> tail ##########################" >> \$outLog
echo " " >> \$outLog
tail -300 \$log >> \$outLog
exit
EOF
      # execute the shell script, option N should have only 10 characters
      close FH;
      chmod 0755, $scriptName;
      $command = "$exec -hard $resource -l h_cpu=$queTime -N $inDir$subDirNo-$runNo $scriptName";
      #$command = "$exec -hard $resource -l h_cpu=$queTime -N dir$subDirNo-$runNo $scriptName";
      if ($secondPassDone eq "kFALSE") {      # skip if 2nd pass completed
	if ($firstPass eq "kTRUE") { print "firstPass\n"; }
	print "##### ",$command,"\n";
	`$command` unless $opt{t};
      };
      $job++;
    }
  }
}

#///////////////////////////////////////////////////////////////////////////////
#//
#// $Log: doFlowSubmit.pl,v $
#// Revision 1.2  2010/03/05 17:04:37  posk
#// ROOT 5.22 compatable.
#// Moved doFlowEvents.C here from StRoot/macros/analysis/
#//
#//
#///////////////////////////////////////////////////////////////////////////////

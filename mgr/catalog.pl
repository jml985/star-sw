#! /usr/local/bin/perl
#
# Script to print production statistics and produce job script files
#
use strict;
use Sys::Hostname;
#use FileHandle;
use English;
my $caller_name  = 'catalog.pl';
my @PROCESSES    = ('tfs,tss');
my $process;
my $DIR;
my $TOP;
my $PROCESS;
my $SET;
my $EXTENSION;
my $LIST;
my $RAW_SET;
my $DISK1        = "/disk00001";
my $HOSTNAME     = hostname();
my $RCF          = "rcf.rhic.bnl.gov";
#if ($HOSTNAME == $RCF) {$DISK1 = "/net/rmds03/disk1";}
my $TOPHPSS_SINK =  "/home/starsink/raw";
my $TOPHPSS_RECO =  "/home/starreco/reco";
my $TOPDISK1_RECO=  $DISK1 . "/star";
my $TOP_TEST     =  "/star/scr2f/starreco/MDC2/tests";
my $REQUEST      =  "/star/u2e/starreco/MDC2/requests";
my $JOB_LOG      =  $DISK1 . "/star/MDC2";
my $Objy         =  $DISK1 . "/star/stardb/dst";
#my @SETS = @ARGV;
my @SETS=     (
"auau200/venus412/default/b0_3/year_1b/hadronic_on",
"auau200/venus412/default/b0_3/year_2a/hadronic_on",
"auau200/venus412/default/b0_9/year_1b/hadronic_on",
"auau200/venus412/default/b3_6/year_1b/hadronic_on",
"auau200/venus412/default/b6_9/year_1b/hadronic_on",
"auau200/venus412/default/b9_12/year_1b/hadronic_on",
"auau200/venus412/halffield/b0_3/year_1b/hadronic_on",
"auau200/hijing135/default/b0_3/year_1b/hadronic_on",
"auau200/hijing135/default/b0_9/year_2a/hadronic_on",
"auau200/hijing135/jetq_off/b0_3/year_1b/hadronic_on",
"auau200/hijing135/jetq_on/b0_3/year_1b/hadronic_on",
"auau200/hijing135/jetq_on/b0_9/year_1b/hadronic_on",
"auau200/hijing135/jetq_on/b3_6/year_1b/hadronic_on",
"auau200/hijing135/jetq_on/b6_9/year_1b/hadronic_on",
"auau200/hijing135/jetq_on/b9_12/year_1b/hadronic_on",
"auau200/hijing/b0_3/jet05/year_1b/hadronic_on",
"auau200/hijing/b0_3/jet10/year_1b/hadronic_on",
"auau200/hijing/b0_3/jet15/year_1b/hadronic_on",
"auau200/hijing/b0_3/jet20/year_1b/hadronic_on",
"auau200/hijing/default/jet05/year_2a/hadronic_on",
"auau200/hijing/default/jet10/year_2a/hadronic_on",
"auau200/hijing/default/jet15/year_2a/hadronic_on",
"auau200/hijing/default/jet20/year_2a/hadronic_on",
"auau200/hbt_vni/1d/r5/year_1b/hadronic_on",
"auau200/hbt_vni/1d/r7/year_1b/hadronic_on",
"auau200/hbt_vni/sol/r533/year_1b/hadronic_on",
"auau200/hbt_vni/sol/r563/year_1b/hadronic_on",
"auau200/hbt_vni/sol/r674/year_1b/hadronic_on",
"auau200/hijet/default/central/year_1b/hadronic_on",
"auau200/vni/after/central/year_1b/hadronic_on",
"auau200/vni/after/minb/year_1b/hadronic_on",
"auau200/vni/before/central/year_1b/hadronic_on",
"auau200/vni/before/minb/year_1b/hadronic_on",
"pp200/pythia/default/jet15/year_1b/hadronic_on",
"pp200/pythia/default/jet15/year_2a/hadronic_on",
"pp200/pythia/default/minbias/year_1b/hadronic_on",
"pp200/pythia/default/minbias/year_2a/hadronic_on",
"pau200/hijing/default/minbias/year_2a/hadronic_on");
my %geant_size = ();
my %geant_date = (); my %geant_test = ();         # test of input (Y/N)
my %geant_noev = ();
my $hpss_size = 0;
my $hpss_date = 0;
my $disk_size = 0;
my $disk_date = 0;
my %size_tfs = ();           # tfs on hpss
my %date_tfs = ();
my %tfs_size = ();           # tfs dst on disk1
my %tfs_date = ();
my %size_tss = ();           # tss dst on hpss 
my %date_tss = ();
my %tss_size = ();           # tss dst on disk1 
my %tss_date = ();
my %files = ();
my %files_job = ();
my %comment = ();
my %jobfile = ();
my %archive = ();
my %running = ();
my %processed = ();
my %noevents = ();
my %timeperevent = ();
my %Makers = ();
my %REALTIME = ();
my %CPUTIME = ();
my %kBsec = ();
my %node = ();
my %job_status = ();
my @chars;
my %jobs = ();
my %status = ();
my %nodes  = ();
#___________________________________________
my $TOTAL_GEANT_SIZE=0;
my $NO_OF_EVENTS=0;
my $TOTAL_SIZE_TFS = 0;
my $TOTAL_SIZE_TSS = 0;
my $TOTAL_TFS_SIZE = 0;
my $TOTAL_TSS_SIZE = 0;
my $TOTAL_NO_EVE   = 0;
my $set;
my $no_events;
my $no_events_complete;
my $input;
my $input_files;
my $output;
my $output_files;
my $answer;                   # output of system calls
my $catalog;
my %list_of_files;
my $dummy;
my $day;
my $time;
my $time2;
my $filename;
my @lines;
my @words;
my $line;
my $file;
my $dir;                       # current directory
my $tag;                       #
my @out;
my $t;
my $gsze;
my $gdat;
my $JFS;   # job script status XYZ (X = A - archived
           #                            J - in jobfiles
           #                            N - in jobfiles_new
           #                            L - in logfiles
           #                        Y = F - fast simulator
           #                        Z = S - slow simulator
my $dsze;
my $ddat;
my $run;
my $nev;
my $tpe;
my $last_line;
#_________________ parameters for stat
my $dev;  # device number of filesystem
my $ino;  # Inode number
my $mode; # File mode (type and permissions)
my $nlink;# Number of (hard) links to the file
my $uid;  # Numeric user ID of the files's owner
my $gid;  # Numeric group ID of the files's owner
my $rdev; # The device identifier (special files only)
my $size; # Total size of the file, in bytes
my $atime;# Last access time since the epoch
my $mtime;# Last modify time since the epoch
my $ctime;# Inode change time (NOT creation time!) since the epoch
my $blksize;# Preferred blocksize for the file system I/O
my $blocks;# Actual number of blocks allocated
my $now;  # current time
my $full_name;
my $dtime;
$now = time;
#_________________ parameters for localtime
my $sec;
my $min;
my $hour;
my $mday;
my $mon;
my $year;
my $wday;
my $yday;
my $isdst;
my $month;
my $BJOBS;
my $CRS_JOBS;
my $ifnever =0;
#____________________________________________________________________
# Subroutines
#____________________________________________________________________
sub usage
  {
    printf("\nUsage: %s <job id>_<subjob number> | job_<job id>_<subjob number>\n",$0);
    printf("\tWhere <job id> is the job identifier of the job to kill and\n");
    printf("\t<subjob number> is the subjob number of the job to kill.\n");
    printf("\tThe second form with the string \"job_\" prepended is the form\n");
    printf("\tpresented by the \"crs_node_status.pl\" script.\n");
    exit(1);
  }
#____________________________________________________________________
sub get_list_dir($$$){
  my $DIR       = @_[0];
  my $EXTENSION = @_[1];
  my $LIST      = @_[2];
#  printf ("DIR=%s EXTENSION=%s\n", $DIR, $EXTENSION, $LIST);
  $answer = `ftp -i -v rmds01 2121 <<EOF
cd $DIR
mdir $EXTENSION $LIST
EOF
`;
}
#____________________________________________________________________
sub get_job_status{
  my $CRS_JOBS = `crs_status.pl -c`; #print $CRS_JOBS;
  @lines = split /^/m, $CRS_JOBS;
  foreach $line (@lines){
#   open(CRS_STATUS,"crs_status.log") ||  die "Can't open RAW_SET tus.log: $!\n";
#   while ($line=<CRS_STATUS>){
    my $job;
    @words = split ("%",$line);#   print $line, $#words, "\n";
    next if $#words<1;  $job = $words[1]; $jobs{$job} = $job;
    $job_status{$job} = sprintf("%.4s_%.8s",$words[2],$words[3]);
#    printf ("job %s status %s node %s",$jobs{$job},$job_status{$job},$nodes{$job});
  }
}
#____________________________________________________________________
sub init_hash {
  $no_events = 0;
  $no_events_complete = 0;
  %geant_size = ();
  %geant_date = ();
  %geant_test = ();         # test of input (Y/N)
  %geant_noev = ();
  %size_tfs = ();
  %date_tfs = ();
  %tfs_size = ();           # dst on disk1
  %tfs_date = ();
  %tss_size = ();          # dst in Objy
  %tss_date = ();
  %files = ();
  %files_job = ();
  %comment = ();
  %jobfile = ();
  %archive = ();
  %running = ();
  %processed = ();
  %noevents = ();
  %Makers = ();
  %timeperevent = ();
  %REALTIME = ();
  %CPUTIME = ();
  %kBsec = ();
  %node = ();
}
#____________________________________________________________________
sub read_set($$){
  # gstardata data set
  my $SET = @_[0];
  my $flag = @_[1];
  open(SET,"$SET" ) || die "Can't open SET $SET: $!\n";
  while ($line=<SET>){
    #       printf ("%s\n",$line);
    ($dummy, $dummy, $dummy, $dummy, $size, $mon, $day, $time, $filename ) = split (" ",$line);
#    printf ("filename = %s date =  %s %s %s size = %s\n", $filename,$mon, $day, $time, $size );
    $file = $filename;
    $file =~ s/.fzd//g;
    $file =~ s/_dst.root//g;
    #      printf  ("file = %s size = %s\n", $file, $size);
    $files{$file}       = $file;
    $size = $size/1000000;
    if ($flag =~ "g"){
      $geant_size{$file} .= int $size;
      $geant_date{$file} .= $mon  . "_" . $day ;
      $geant_test{$file}  = "N";
      $size_tfs{$file}   = "";
      $date_tfs{$file}   = "";
      $size_tss{$file}   = "";
      $date_tss{$file}   = "";
      $tfs_size{$file}    = "";
      $tfs_date{$file}    = "";
      $tss_size{$file}   = "";
      $tss_date{$file}   = "";
      $kBsec{$file}       = "";
      $timeperevent{$file} = "";
      $comment{$file}     = "";
      $jobfile{$file}     = "N";
      $archive{$file}     = "N";
      $running{$file}     = "";
      my $no_ev;
      ($dummy, $dummy, $no_ev) = split /_/, $file;
      $no_ev =~ s/evts//g;# printf ("file = %s noev = %d\n", $file, $no_ev);
      $geant_noev{$file}    = $no_ev;
#      printf ("geant %i %s\n",$geant_size{$file}, $geant_date{$file});
    }
    if ($flag =~ "f"){
      $size_tfs{$file}    = int $size;
      $date_tfs{$file}    = $mon  . "_" . $day ;
#      printf ("tfs %i %s\n",$tfs_size{$file}, $tfs_date{$file});
    }
    if ($flag =~ "s"){
      $size_tss{$file}    = int $size;
      $date_tss{$file}    = $mon  . "_" . $day ;
#      printf ("tss %i %s\n",$tss_size{$file}, $tss_date{$file});
    }
  }
  close(SET);
}
#______________________________________________________________________
sub begin_html($){
  my $new_html = @_[0];
  open (HTML,">$new_html");
  print HTML "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML//EN\">\n";
  print HTML "<html>\n";
  print HTML "  <head>\n";
  print HTML "          <title>MDC2 summary</title>\n";
  print HTML "  </head>\n";
  print HTML "  <body BGCOLOR=\"#ccffff\"> \n";
  print HTML "      <h1>Production summary</h1>\n";
  print HTML "<TABLE BORDER=5 CELLSPACING=1 CELLPADDING=2 >\n";
  print HTML "<TR>\n";
  print HTML "<TR ALIGN=CENTER VALIGN=CENTER>\n";
  print HTML "<TD WIDTH=\"30%\" HEIGHT=110><B>Set</B></TD>\n";
  print HTML "<TD WIDTH=\"10%\" HEIGHT=110><B>GEANT input<br>size (GB)</B></TD>\n";
  print HTML "<TD WIDTH=\"10%\" HEIGHT=110><B>GEANT input <br>No. of events</B></TD>\n";
  print HTML "<TD WIDTH=\"10%\" HEIGHT=110><B>tfs/tss dst on HPSS<br>size (GB)</B></TD>\n";
  print HTML "<TD WIDTH=\"10%\" HEIGHT=110><B>tfs/tss dst on disk<br>size (GB)</B></TD>\n";
  print HTML "<TD WIDTH=\"10%\" HEIGHT=110><B>Total DST<br>No. of events<br></B></TD>\n";
  print HTML "</TR>\n";
}
#______________________________________________________________________
sub end_html($){
  my $new_html = @_[0];
  my $Date = `date`;
  
  print HTML "</TABLE>\n";
  print HTML "      <h5>\n";
  print HTML "      <address><a href=\"mailto:didenko\@bnl.gov\">Lidia Didenko</a></address>\n";
  print HTML "<!-- Created: Tue Oct 6 05:29:25 MET 1998 -->\n";
  print HTML "<!-- hhmts start -->\n";
  print HTML "Last modified: $Date\n";
  print HTML "<!-- hhmts end -->\n";
  print HTML "  </body>\n";
  print HTML "</html>\n";
  close (HTML);
}

#____________________________________________________________________
sub logfile($$){
  my $f      = @_[0];
  my $jb_log = @_[1];
  my @events = `grep 'Done with Event no.' $jb_log;`;
#  print "No. of Event", $#events, "\n";
#  print @events;
  $noevents{$f} = $#events+1;
  $no_events += $noevents{$f};
  my @output = `tail -40 $jb_log`;
#  print @output;
  my @string;
  @string = grep /Run completed/, @output;
  if ($string[0]){
    $running{$f} = "done";
    $no_events_complete += $noevents{$f};
    foreach my $line (@output){
      if ($line =~ "Cpu Time = ") {
	@words = split (":",$line);
	my $Maker = $words[0];
#	print "Maker :", $Maker, "\n" ,$words[1];
	my @w = split(" ",$words[1]);
	my $real_time = $w[3];
	my $cpu_time  = $w[8];
	if (!$Makers{$Maker}) {
	  $Makers{$Maker} = $Maker;
	  $REALTIME{$Maker} = 0;
	  $CPUTIME{$Maker} = 0;
	}
	$REALTIME{$Maker} =  $real_time;
	$CPUTIME{$Maker}  += $cpu_time;
#	if ($Maker =~ "l3Tracks") {printf ("Maker : %s  real / cpu = %f %f \n", $Maker, $real_time, $cpu_time);}
#	printf ("%s :: %f  - %f \n",$Maker,$REALTIME{$Maker},$CPUTIME{$Maker});
      }
    } 
  }
  @string = grep /Break/, @output;
  if ($string[0]){
    $comment{$f} .= $string[0];
#    print "------------->", $comment{$file};
  }
}
#____________________________________________________________________
sub crs_script($$$$){
  my $file    = @_[0];
  my $gen_set = @_[1];
  my $set     = @_[2];
  my $process = @_[3];
  my $jb_log  = $JOB_LOG . "/" . $process . "/" . $gen_set . "_" . $file;
  my $jb_arc  = $REQUEST . "/" . $process . "/archive/"  .  $gen_set . "_" . $file;
  my $jb_new  = $REQUEST . "/" . $process . "/new_jobs/" .  $gen_set . "_" . $file;
  my $jb_tom  = $REQUEST . "/" . $process . "/jobfiles/" .  $gen_set . "_" . $file;
  my $jb_hold = $REQUEST . "/" . $process . "/hold_jobs/"  .  $gen_set . "_" . $file;
#  printf ("log %s\n",$jb_log);
  my $done=0;
  if ($size_tfs{$file}) {$done++;}
  if ($tfs_size{$file})  {$done++;}
  if ($tss_size{$file})  {$done++;}
  if (-f $jb_log) {
    if ($running{$file}) {$JFS  .= "R"; $done++;}
    else {                $JFS  .= "L"; $done++; logfile($file,$jb_log);}
  }
  if (!-f $jb_log && $running{$file}) {$JFS  .= "T"; $done++;}
  if (-f $jb_tom) {$JFS  .= "J"; $done++;}
  if (-f $jb_arc) {$JFS  .= "A"; $done++;}
  if (-f $jb_new) {$JFS  .= "N"; $done++;}
  if (-f $jb_hold) {$JFS .= "H"; $done++;}
  
    
#    printf ("create new job file %s\n",$jb_new);
  if (! $done) {
      my $hpss_raw_dir  = $TOPHPSS_SINK . "/" . $set . "/gstardata";
      my $hpss_raw_file = $file . ".fzd";
      my $hpss_dst_dir  = $TOPHPSS_RECO . "/" . $set . "/" . $process;
      my $hpss_dst_file0 = $file . ".root";
      my $hpss_dst_file1 = $file . "_dst.root";
      my $hpss_dst_file2 = $file . "_dst.xdf";
      my $executable    = $REQUEST . "/" . $process . "/" . $process;
#      my $executable    = "bfc_" . $process . ".C";
      my $log_dir       = $JOB_LOG . "/" . $process;
      my $log_name      = $gen_set . "_" . $file;
      my $err_log       = $log_name . ".err";
      if (!open (TOM_SCRIPT,">$jb_new")) {printf ("Unable to create job submission script %s\n",$jb_new);}
      print TOM_SCRIPT "mergefactor=1\n";
      print TOM_SCRIPT "#input\n";
      print TOM_SCRIPT "      inputnumstreams=1\n";
      print TOM_SCRIPT "      inputstreamtype[0]=HPSS\n";
      print TOM_SCRIPT "      inputdir[0]=$hpss_raw_dir\n";
      print TOM_SCRIPT "      inputfile[0]=$hpss_raw_file\n";
      print TOM_SCRIPT "#output\n";
      print TOM_SCRIPT "      outputnumstreams=3\n";
      print TOM_SCRIPT "#output stream \n";
      print TOM_SCRIPT "      outputstreamtype[0]=HPSS\n";
      print TOM_SCRIPT "      outputdir[0]=$hpss_dst_dir\n";
      print TOM_SCRIPT "      outputfile[0]=$hpss_dst_file0\n";
      print TOM_SCRIPT "      outputstreamtype[1]= HPSS\n";
      print TOM_SCRIPT "      outputdir[1]=$hpss_dst_dir\n";
      print TOM_SCRIPT "      outputfile[1]=$hpss_dst_file1\n";
      print TOM_SCRIPT "      outputstreamtype[2]=HPSS\n";
      print TOM_SCRIPT "      outputdir[2]=$hpss_dst_dir\n";
      print TOM_SCRIPT "      outputfile[2]=$hpss_dst_file2\n";
      print TOM_SCRIPT "#standard out -- Should be five outputs\n";
      print TOM_SCRIPT "      stdoutdir=$log_dir\n";
      print TOM_SCRIPT "      stdout=$log_name\n";
      print TOM_SCRIPT "#standard error -- Should be five\n";
      print TOM_SCRIPT "      stderrdir=$log_dir\n";
      print TOM_SCRIPT "      stderr=$err_log\n";
      print TOM_SCRIPT "      notify=starreco\@rcf.rhic.bnl.gov\n";
      print TOM_SCRIPT "#program to run\n";
      print TOM_SCRIPT "      executable=$executable\n";
      close(TOM_SCRIPT);
      $JFS .= "N";
    }
}
#____________________________________________________________________
format STDOUT_TOP =
set = b0_3/year_1b/hadronic_off Input Total = 249.671 (GB) in 256 files -> Output Total = 0(GB) in 0 files
set = @<<<<<<<<<<<<<<<<<<<<<<<< Input Total = @<<<<<< (GB) in @<< files -> Output Total = @>> (GB) in @>> files
      $set,                                   $input,         $input_files,               $output,    $output_files
.
format STDOUT_TOP =
set =@<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
$set,
                      size               tfs/tss on HPSS  tfs/tss on disk        No. time   Rate  Comments
            Filename  (MB)   date   JSF  (MB)   date     (MB)   date     run     ev.(mins) (kB/s) cpu\tot (hrs)
.
format STDOUT =
@<<<<<<<<<<<<<<<<<<<<@>>>> @>>>>>@||||||||@>>>>>@>>>>>>>@>>>>>>>@>>>>>>>>@>>>>@>>>>>>@<<<<<<<<<<@>>>@>>>> @<<<< @<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
$file,$geant_size{$file},$geant_date{$file},$JFS,$hpss_size,$hpss_date,$disk_size,$disk_date,$running{$file},$noevents{$file},$timeperevent{$file},$kBsec{$file},$comment{$file}
.
#____________________________________________________________________
format OUT_TOP =
set = b0_3/year_1b/hadronic_off Input Total = 249.671 (GB) in 256 files -> Output Total = 0(GB) in 0 files
set = @<<<<<<<<<<<<<<<<<<<<<<<< Input Total = @<<<<<< (GB) in @<< files -> Output Total = @>> (GB) in @>> files
      $set,                                   $input,         $input_files,               $output,    $output_files
.
format OUT_TOP =
set =@<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
$set,
                      size               tfs/tss on HPSS  tfs/tss on disk        No. time   Rate  Comments
            Filename  (MB)   date   JSF  (MB)   date     (MB)   date     run     ev.(mins) (kB/s) cpu\tot (hrs)
.
format OUT =
@<<<<<<<<<<<<<<<<<<<<@>>>> @>>>>>@||||||||@>>>>>@>>>>>>>@>>>>>>>@>>>>>>>>@>>>>@>>>>>>@<<<<<<<<<<@>>>@>>>> @<<<< @<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
$file,$geant_size{$file},$geant_date{$file},$JFS,$hpss_size,$hpss_date,$disk_size,$disk_date,$running{$file},$noevents{$file},$timeperevent{$file},$kBsec{$file},$comment{$file}
.
#______________________________________________________________
#  @lines = split /^/m, `date +"%T_%d_%h_%y"`;
#  my $DATE = /$/,`date +"%T_%d_%h_%y"`;
   ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = gmtime(time);
#   printf("%s %s %s %s 5s %s %s %s\n", $sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst);
#   my $month =  ("Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec")[(localtime)[3]];
   my $DATE  = $hour . ":" . $min . ":" . $sec . "_" . $day . $mon . "_" . $year;
#   printf ("Date = %s\n",$DATE);
my $html     = "MDC2.summary.html";
my $new_html = $html . $DATE;
get_job_status();
begin_html($new_html);
#foreach my $full_set ("auau200/hijing135/jetq_on/b9_12/year_1b/hadronic_on"){
foreach my $full_set (@SETS){
  $set = $full_set;
  $set =~ s/auau200\///g;
  init_hash();
  my $gen_set = $set;
  $gen_set =~ s/\//_/g;
  printf ("set = %s \n", $set);
# gstardata
  my $gstardata_set = $gen_set . ".list";
  printf ("gen_set = %s gstardata_set = %s \n", $gen_set, $gstardata_set);
  $DIR = $TOPHPSS_SINK . "/" . $full_set . "/" . "gstardata";
  $EXTENSION = "*.fz*";
  $LIST = $gstardata_set;
  if (! -e $gstardata_set) {get_list_dir($DIR,$EXTENSION,$LIST);}
  read_set($gstardata_set,'g');
# tfs
  my $tfs_set = "tfs_" . $gen_set . ".list";
  printf ("gen_set = %s tfs_set = %s \n", $gen_set, $tfs_set);
  $DIR = $TOPHPSS_RECO . "/" . $full_set . "/" . "tfs";
  $EXTENSION = "*_dst.root";
  $LIST = $tfs_set;
  if (! -e $tfs_set) {get_list_dir($DIR,$EXTENSION,$LIST);}
  read_set($tfs_set,'f');
# tss
  my $tss_set = "tss_" . $gen_set . ".list";
  printf ("gen_set = %s tss_set = %s \n", $gen_set, $tss_set);
  $DIR = $TOPHPSS_RECO . "/" . $full_set . "/" . "tss";
  $EXTENSION = "*_dst.root";
  $LIST = $tss_set;
  if (! -e $tss_set) {get_list_dir($DIR,$EXTENSION,$LIST);}
  read_set($tss_set,'s');

  my $no_of_files=0;
  my $no_of_events=0;
  my $total_geant_size =0;
  my $total_size_tfs = 0;
  my $total_tfs_size = 0; # on disk1
  my $total_tss_size = 0;
  my $total_size_tfs = 0; # on hpss
  my $total_size_tss = 0;
  my $total_no_eve   = 0;
#  select 
  my $catalog = $gen_set . ".catalog";
  open (OUT, ">$catalog");
  foreach  $file (sort keys %files){
# tfs/tss on hpss

# tfs on disk1
    my $full_name =  $TOPDISK1_RECO . "/" . $full_set . "/tfs/" . $file . "_dst.root";
#    printf ("full name %s\n",$full_name);
    if (-f $full_name){
      $line = `ls -l  $full_name`;# printf ("line = %s\n", $line);
      ($dummy, $dummy, $dummy, $dummy, $size, $mon, $day, $time, $filename ) = split (" ",$line);
#      printf ("file = %s size = %d date %s %s \n", $full_name, $size, $mon, $day);
      $size = $size/1000000;
      $tfs_size{$file} = int $size;
      $tfs_date{$file} = $mon . "_" . $day;
      $total_tfs_size += $tfs_size{$file};
    }
# tss on disk1
    my $full_name =  $TOPDISK1_RECO . "/" . $full_set . "/tss/" . $file . "_dst.root";
#    printf ("full name %s\n",$full_name);
    if (-f $full_name){
      $line = `ls -l  $full_name`;# printf ("line = %s\n", $line);
      ($dummy, $dummy, $dummy, $dummy, $size, $mon, $day, $time, $filename ) = split (" ",$line);
#      printf ("file = %s size = %d date %s %s \n", $full_name, $size, $mon, $day);
      $size = $size/1000000;
      $tss_size{$file} = int $size;
      $tss_date{$file} = $mon . "_" . $day;
      $total_tss_size += $tss_size{$file};
      
    }
    $no_of_files++;
    $total_geant_size += $geant_size{$file};
    $no_of_events     += $geant_noev{$file};
    my $tag = $gen_set . "_" . $file;
#    printf ("tag %s status %s\n",$tag,$job_status{$tag});
    $running{$file} = $job_status{$tag};
    $JFS = "";
    $JFS .= "f";
    my $process = "tfs";
    crs_script($file,$gen_set,$full_set,$process);
#    if ($no_events_complete) {last;}
    $JFS .= "-s";
    my $process = "tss";
    crs_script($file,$gen_set,$full_set,$process);
    $hpss_size = $size_tfs{$file} . "/" . $size_tss{$file} . "|" ;
    $hpss_date = $date_tfs{$file} . "/" . $date_tss{$file} . "|";
    $disk_size = $tfs_size{$file} . "/" . $tss_size{$file} . "|" ;
    $disk_date = $tfs_date{$file} . "/" . $tss_date{$file} . "|";
    $total_size_tfs += $size_tfs{$file};
    $total_size_tss += $size_tss{$file};
    $total_no_eve   += $noevents{$file};
    write OUT;
    write;
  }
  printf (" Total no. of files %i with %12.3f (GBytes) with %i events on input and %i/%i events on output\n",
  $no_of_files, $total_geant_size/1000., $no_of_events,$no_events,$no_events_complete);
  printf (" Total no. of event processed = %i on hpss %12.3f/%12.3f on disk %12.3f/%12.3f\n",
	  $total_no_eve,$total_size_tfs,$total_size_tss,$total_tfs_size,$total_tss_size);
#  my $HPSS_GB = 
  my $GB = $total_geant_size/1000;
  print HTML "<TR ALIGN=CENTER VALIGN=CENTER>\n";
  print HTML "<td><a href=\"$catalog\">$set</a></td>\n";
  print HTML "<td>$GB</td><td>$no_of_events</td><td>$total_size_tfs/$total_size_tss</td><td>$total_tfs_size/$total_tss_size</td><td>$total_no_eve</td></tr>\n"; 
  my $prt=0;
  if ($prt && $no_events_complete) {
    my $real_time_total = 0;
    my $cpu_time_total = 0;
    foreach my $Maker (sort keys %Makers){
      my $real_time = int $REALTIME{$Maker};
      my $cpu_time  = int $CPUTIME{$Maker};
      $real_time_total += $real_time;
      $cpu_time_total  += $cpu_time;
#      printf ("Total %s %f seconds %f seconds\n",  $Maker, $real_time,$cpu_time); 
      my $real_time =  $real_time/$no_events_complete;
      my $cpu_time  =  $cpu_time/$no_events_complete;
      printf ("%s %f seconds %f seconds\n",  $Maker, $real_time,$cpu_time); 
      
    }
    $real_time_total = $real_time_total/$no_events_complete;
    $cpu_time_total  = $cpu_time_total /$no_events_complete;
    printf ("Total %f seconds %f seconds\n",  $real_time_total,$cpu_time_total); 
  }
  close (OUT);
  $TOTAL_GEANT_SIZE += $total_geant_size; 
  $NO_OF_EVENTS += $no_of_events;
  $TOTAL_SIZE_TFS += $total_size_tfs;
  $TOTAL_SIZE_TSS += $total_size_tss;
  $TOTAL_TFS_SIZE += $total_tfs_size;
  $TOTAL_TSS_SIZE += $total_tss_size;
  $TOTAL_NO_EVE   += $total_no_eve;
#____$TOTAL_SIZE_TSS__________________________________________________________
}
  print HTML "<TR ALIGN=CENTER VALIGN=CENTER>\n";
  print HTML "<td>Total</td>\n";
  my $GB = $TOTAL_GEANT_SIZE/1000;
  print HTML "<td>$GB</td><td>$NO_OF_EVENTS</td><td>$TOTAL_SIZE_TFS/$TOTAL_SIZE_TSS</td><td>$TOTAL_TFS_SIZE/$TOTAL_TSS_SIZE</td><td>$TOTAL_NO_EVE</td></tr>\n";
  print HTML "</TR>\n";

end_html($new_html);
my $answer = `rm -f $html; ln -s $new_html $html;`; 

exit(0);
# last line
  

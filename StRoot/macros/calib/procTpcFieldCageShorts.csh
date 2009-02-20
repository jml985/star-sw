#!/bin/csh

##################################
#
#  procTpcFieldCageShorts.csh
#  Author: G. Van Buren - BNL
#  Created: April, 2007
#  Modified: Dec. 9, 2007
#  Modified: Feb. 20, 2009
#
#  Calculates TPC FC missing resistance and loads values into DB
#  Requires use of matchTpcFieldCageShorts.C
#
##################################

setenv STAR_LEVEL dev
source ${GROUP_DIR}/.stardev

set ID=`/usr/bin/whoami`
echo "procTpcFieldCageShorts: running at `/bin/date`"

set tpc_db = "-h onldb.starp.bnl.gov --port=3502"
set daq_db = "-h onldb.starp.bnl.gov --port=3501"
set fcs_db = "-h dbx.star.bnl.gov --port=3316"

set qq = "/tmp/FCS_short1_$ID.txt"
set q2 = "/tmp/FCS_short2_$ID.txt"
set dd = "currents.txt"
set rr = "run_times.txt"
set ff = "dbout.txt"
set outdir = /star/data10/calib/log/FCS
set logdir = `/bin/date --utc '+%Y%m%d.%H%M%S'`
@ mingap = 240

if ( -x /usr/bin/mysql ) then
set MYSQL=/usr/bin/mysql
else
set MYSQL=mysql
endif

set cdir = $cwd
/bin/mkdir -p $logdir $outdir
cd $logdir
/bin/touch log

# floats can handle 6 months worth of seconds, but not a year or more!
# 16777216 seconds = ~194 days
# last entry in Calibrations_tpc/TpcSpaceCharge used to define yearstart
/bin/cat >! $qq <<EOF
select unix_timestamp(beginTime) from TpcSpaceCharge order by beginTime desc limit 1;
EOF
@ yearstart = `/bin/cat $qq | $MYSQL $fcs_db -C Calibrations_tpc | /bin/grep -v begin`

# get info on last time this script was run (recreate that info from DB)
/bin/cat >! $qq <<EOF
select resistor,MissingResistance,unix_timestamp(beginTime)
from tpcFieldCageShort where side=1 and cage=1 order by beginTime desc limit 1;
EOF
/bin/cat $qq >> log; echo "" >> log
/bin/cat $qq | $MYSQL $fcs_db -C Geometry_tpc | /bin/grep -v resist >! $q2
set last_resA = `/bin/awk '{print $1","$2}' $q2`
@ lasttime = `/usr/bin/cut -f 3 $q2`
if ($lasttime < $yearstart) @ lasttime = $yearstart
set query = "select dataID from tpcFieldCageShort order by dataID desc limit 1;"
set last_resB = `echo $query | $MYSQL $fcs_db -C Geometry_tpc | /bin/grep -v dataID`
set last_res = "$last_resA,$last_resB"

@ thistime = `/bin/date '+%s'`
@ thistime2 = $thistime - $yearstart
set tlimited = "between from_unixtime($lasttime) and from_unixtime($thistime)"
set daqtables = "daqSummary as ds left join detectorSet as de on ds.runNumber=de.runNumber"

# determine TPC detectorID
set query = "select detectorID from detectorTypes where name='tpc'"
set tpcid = `echo $query | $MYSQL $daq_db -C RunLog | /bin/grep -v det`

# query for runs
/bin/cat >! $qq <<EOF
select ds.runNumber,from_unixtime(ds.firstEventTime),
ds.firstEventTime-$yearstart,ds.lastEventTime-$yearstart
from $daqtables
where ds.beginTime $tlimited
and de.detectorID=$tpcid
and ds.firstEventTime>0 and ds.lastEventTime>ds.firstEventTime
order by ds.firstEventTime asc;
EOF
/bin/cat $qq >> log; echo "" >> log
/bin/cat $qq | $MYSQL $daq_db -C RunLog | /bin/grep -v runNumber >! $rr
set runs   = `/usr/bin/cut -f 1 $rr`
if ($#runs == 0) then
  cd $cdir; /bin/rm -rf $logdir $qq
  exit
endif
set begins = `/usr/bin/cut -f 3 $rr`
set ends   = `/usr/bin/cut -f 4 $rr`
set latestrun = $runs[$#runs]

# determine if we're in the middle of a tpc run now
/bin/cat >! $qq <<EOF
select run.idx_rn from run left join dets on run.idx_rn=dets.idx_rn
where dets.idx_det=$tpcid and run.run_number>$latestrun;
EOF
/bin/cat $qq >> log; echo "" >> log
set running_now = `/bin/cat $qq | $MYSQL $daq_db -C Conditions_rts | /usr/bin/wc -l`
echo "running_now = $running_now" >> log
echo "procTpcFieldCageShorts: $latestrun , running_now = $running_now"

# look back for last long gap (> mingap) between runs
# if we're not in the middle of a tpc run now, consider gap between now and last run
@ gap_time = 0
if ($running_now == 0) then
  @ buffer_time = $ends[$#ends] + $mingap
  if ($buffer_time < $thistime2) @ gap_time = $buffer_time
endif
@ index = $#ends
while ($gap_time == 0 && $index > 1)
  @ index2 = $index - 1
  @ buffer_time = $ends[$index2] + $mingap
  if ($buffer_time < $begins[$index]) @ gap_time = $buffer_time
  @ index = $index2
end

# exit if no gaps found
if ($gap_time == 0) then
  cd $cdir; /bin/rm -rf $logdir $qq
  exit
endif
echo "gap_time = $gap_time" >> log

# new time limits
@ thistime = $gap_time + $yearstart - 60
echo "thistime = $thistime" >> log
set tlimited = "between from_unixtime($lasttime) and from_unixtime($thistime)"

# query for currents
# inner/outer appear switched, handled in matchTpcFieldCageShorts.C
# place an extra 0 to represent external resistor (for OFCW usage)
/bin/cat >! $qq <<EOF
select unix_timestamp(beginTime)-$yearstart,currentInnerFieldCageEast_1,
currentOuterFieldCageEast_1,currentInnerFieldCageWest_1,currentOuterFieldCageWest_1,0
from tpcFieldCage where beginTime $tlimited order by beginTime;
EOF

/bin/cat $qq >> log; echo "" >> log
/bin/cat $qq | $MYSQL $tpc_db -C Conditions_tpc | /bin/grep -v begin >! $dd

# Reformat dates
/bin/sed -i 's/[-:]/ /g' $rr

# Run the matching code
set entryDate = `/bin/date --utc '+%Y-%m-%d'`
set entryTime = `/bin/date --utc '+%T'`

$ROOTSYS/bin/root -l -b -q $HOME/scripts/matchTpcFieldCageShorts.C+\(\"${entryDate}\ ${entryTime}\",${last_res}\) >>& log

@ ndbent = `/bin/cat $ff | /usr/bin/wc -l`

if ($ndbent > 0) then
  setenv DB_ACCESS_MODE write
  $STAR/.$STAR_HOST_SYS/bin/root4star -l -b -q $HOME/scripts/LoadShorts.C\(\"$ff\"\) >>& log
endif

cd $cdir
/bin/tar cf $outdir/$logdir.tar $logdir
/usr/bin/gzip $outdir/$logdir.tar
/bin/rm -rf $logdir

#####################################
# $Id: procTpcFieldCageShorts.csh,v 1.1 2009/02/20 18:50:58 genevb Exp $
# $Log: procTpcFieldCageShorts.csh,v $
# Revision 1.1  2009/02/20 18:50:58  genevb
# Placement in CVS of automatic TpcFieldCageShort calib codes
#
#


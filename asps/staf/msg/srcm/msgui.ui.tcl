# interface generated by SpecTcl version 0.3 from msgui.ui
#   root     is the parent window for this user interface

proc msgui_ui {root args} {

	# this treats "." as a special case

	if {$root == "."} {
		set base ""
	} else {
		set base $root
	}
	
	frame $base.frame#1 \
		-background #d88700

	message $base.message#1 \
		-anchor sw \
		-aspect 100000 \
		-background #d88700 \
		-borderwidth 1 \
		-foreground black \
		-highlightbackground #d88700 \
		-padx 7 \
		-pady 3 \
		-text {
}
	catch {
		$base.message#1 configure \
			-font -*-screen-Medium-R-Normal-*-*-140-*-*-*-*-*-*
	}

	listbox $base.listbox#1 \
		-background #9d3d00 \
		-borderwidth 6 \
		-foreground white \
		-height 16 \
		-highlightbackground #ffa831 \
		-selectbackground #FFA831 \
		-width 133 \
		-yscrollcommand "$base.scrollbar#1 set"
	catch {
		$base.listbox#1 configure \
			-font -*-Screen-Medium-R-Normal-*-*-140-*-*-*-*-*-*
	}

	scrollbar $base.scrollbar#1 \
		-activebackground #ffa831 \
		-activerelief sunken \
		-background #9d3d00 \
		-command "$base.listbox#1 yview" \
		-highlightbackground #d88700 \
		-orient v \
		-troughcolor #D88700

	label $base.label#1 \
		-background #d88700 \
		-foreground black \
		-highlightbackground #d88700 \
		-justify left \
		-text {Prefix        }
	catch {
		$base.label#1 configure \
			-font -*-Helvetica-Bold-R-Normal-*-*-100-*-*-*-*-*-*
	}

	entry $base.entry#1 \
		-background #9d3d00 \
		-cursor {} \
		-foreground white \
		-highlightbackground #d88700 \
		-textvariable Prefix \
		-width 71
	catch {
		$base.entry#1 configure \
			-font -*-screen-Medium-R-Normal-*-*-140-*-*-*-*-*-*
	}

	label $base.label#9 \
		-background #d88700 \
		-highlightbackground #d88700 \
		-text shmid
	catch {
		$base.label#9 configure \
			-font -*-Helvetica-Bold-R-Normal-*-*-100-*-*-*-*-*-*
	}

	message $base.message#4 \
		-aspect 1000 \
		-background #9d3d00 \
		-foreground white \
		-highlightbackground #D88700 \
		-padx 7 \
		-pady 3 \
		-relief ridge \
		-textvariable Shmid
	catch {
		$base.message#4 configure \
			-font -*-screen-Medium-R-Normal-*-*-140-*-*-*-*-*-*
	}

	button $base.button#1 \
		-activebackground #ffa831 \
		-background #d88700 \
		-command {MsgSummary .} \
		-highlightbackground #d88700 \
		-padx 10 \
		-pady 3 \
		-text {   Summary    }
	catch {
		$base.button#1 configure \
			-font -*-Helvetica-Bold-R-Normal-*-*-100-*-*-*-*-*-*
	}

	label $base.label#2 \
		-background #d88700 \
		-highlightbackground #d88700 \
		-text {Count Limit}
	catch {
		$base.label#2 configure \
			-font -*-Helvetica-Bold-R-Normal-*-*-100-*-*-*-*-*-*
	}

	entry $base.entry#2 \
		-background #9d3d00 \
		-cursor {} \
		-foreground white \
		-highlightbackground #d88700 \
		-textvariable CountLimit \
		-width 8
	catch {
		$base.entry#2 configure \
			-font -*-screen-Medium-R-Normal-*-*-140-*-*-*-*-*-*
	}

	checkbutton $base.checkbutton#1 \
		-activebackground #ffa831 \
		-activeforeground black \
		-background #d88700 \
		-foreground black \
		-highlightbackground #d88700 \
		-relief raised \
		-text {Active        } \
		-variable Active
	catch {
		$base.checkbutton#1 configure \
			-font -*-Helvetica-Bold-R-Normal-*-*-100-*-*-*-*-*-*
	}

	button $base.button#5 \
		-activebackground #ffa831 \
		-background #d88700 \
		-command {MsgPrefixGet .} \
		-highlightbackground #d88700 \
		-padx 10 \
		-pady 3 \
		-text {  Get  }
	catch {
		$base.button#5 configure \
			-font -*-Helvetica-Bold-R-Normal-*-*-100-*-*-*-*-*-*
	}

	message $base.message#2 \
		-aspect 1000 \
		-background #9d3d00 \
		-foreground white \
		-highlightbackground #D88700 \
		-padx 7 \
		-pady 3 \
		-relief ridge
	catch {
		$base.message#2 configure \
			-font -*-screen-Medium-R-Normal-*-*-140-*-*-*-*-*-*
	}

	button $base.button#10 \
		-activebackground #ffa831 \
		-background #d88700 \
		-command {MsgRemoveShmid .} \
		-highlightbackground #d88700 \
		-justify left \
		-padx 10 \
		-pady 3 \
		-text {remove shmid}
	catch {
		$base.button#10 configure \
			-font -*-Helvetica-Bold-R-Normal-*-*-100-*-*-*-*-*-*
	}

	button $base.button#3 \
		-activebackground #ffa831 \
		-background #d88700 \
		-command {MsgSummaryCPU .} \
		-highlightbackground #d88700 \
		-justify left \
		-padx 10 \
		-pady 3 \
		-text {   CPU Usage }
	catch {
		$base.button#3 configure \
			-font -*-Helvetica-Bold-R-Normal-*-*-100-*-*-*-*-*-*
	}

	label $base.label#3 \
		-background #d88700 \
		-highlightbackground #d88700 \
		-text {Abort Limit}
	catch {
		$base.label#3 configure \
			-font -*-Helvetica-Bold-R-Normal-*-*-100-*-*-*-*-*-*
	}

	entry $base.entry#3 \
		-background #9d3d00 \
		-cursor {} \
		-foreground white \
		-highlightbackground #d88700 \
		-textvariable AbortLimit \
		-width 8
	catch {
		$base.entry#3 configure \
			-font -*-screen-Medium-R-Normal-*-*-140-*-*-*-*-*-*
	}

	checkbutton $base.checkbutton#2 \
		-activebackground #ffa831 \
		-activeforeground black \
		-background #d88700 \
		-foreground black \
		-highlightbackground #d88700 \
		-relief raised \
		-text {Counting   } \
		-variable Counting
	catch {
		$base.checkbutton#2 configure \
			-font -*-Helvetica-Bold-R-Normal-*-*-100-*-*-*-*-*-*
	}

	button $base.button#6 \
		-activebackground #ffa831 \
		-background #d88700 \
		-command {MsgPrefixSet .} \
		-highlightbackground #d88700 \
		-padx 10 \
		-pady 3 \
		-text {  Set  }
	catch {
		$base.button#6 configure \
			-font -*-Helvetica-Bold-R-Normal-*-*-100-*-*-*-*-*-*
	}

	entry $base.entry#10 \
		-background #9d3d00 \
		-cursor {} \
		-foreground white \
		-highlightbackground #d88700 \
		-textvariable NodeName \
		-width 12
	catch {
		$base.entry#10 configure \
			-font -*-screen-Medium-R-Normal-*-*-140-*-*-*-*-*-*
	}

	checkbutton $base.checkbutton#4 \
		-activebackground #ffa831 \
		-activeforeground black \
		-background #d88700 \
		-foreground black \
		-highlightbackground #d88700 \
		-relief raised \
		-text {List Inactives} \
		-variable ListInactives
	catch {
		$base.checkbutton#4 configure \
			-font -*-Helvetica-Bold-R-Normal-*-*-100-*-*-*-*-*-*
	}

	label $base.label#5 \
		-background #d88700 \
		-highlightbackground #d88700 \
		-text {Alarm Level}
	catch {
		$base.label#5 configure \
			-font -*-Helvetica-Bold-R-Normal-*-*-100-*-*-*-*-*-*
	}

	entry $base.entry#5 \
		-background #9d3d00 \
		-cursor {} \
		-foreground white \
		-highlightbackground #d88700 \
		-textvariable AlarmLevel \
		-width 8
	catch {
		$base.entry#5 configure \
			-font -*-screen-Medium-R-Normal-*-*-140-*-*-*-*-*-*
	}

	checkbutton $base.checkbutton#3 \
		-activebackground #ffa831 \
		-activeforeground black \
		-background #d88700 \
		-foreground black \
		-highlightbackground #d88700 \
		-justify left \
		-relief raised \
		-text {Alarming   } \
		-variable Alarming
	catch {
		$base.checkbutton#3 configure \
			-font -*-Helvetica-Bold-R-Normal-*-*-100-*-*-*-*-*-*
	}

	button $base.button#7 \
		-activebackground #ffa831 \
		-background #d88700 \
		-command {MsgPrefixDelete .} \
		-highlightbackground #d88700 \
		-padx 10 \
		-pady 3 \
		-text Delete
	catch {
		$base.button#7 configure \
			-font -*-Helvetica-Bold-R-Normal-*-*-100-*-*-*-*-*-*
	}

	label $base.label#8 \
		-background #d88700 \
		-highlightbackground #d88700 \
		-text {Page Length}
	catch {
		$base.label#8 configure \
			-font -*-Helvetica-Bold-R-Normal-*-*-100-*-*-*-*-*-*
	}

	entry $base.entry#9 \
		-background #9d3d00 \
		-cursor {} \
		-foreground white \
		-highlightbackground #d88700 \
		-textvariable PageLength \
		-width 3
	catch {
		$base.entry#9 configure \
			-font -*-screen-Medium-R-Normal-*-*-140-*-*-*-*-*-*
	}

	button $base.button#9 \
		-activebackground #ffa831 \
		-background #d88700 \
		-command {MsgLoadFrom .} \
		-highlightbackground #d88700 \
		-justify left \
		-padx 10 \
		-pady 3 \
		-text Load
	catch {
		$base.button#9 configure \
			-font -*-Helvetica-Bold-R-Normal-*-*-100-*-*-*-*-*-*
	}

	entry $base.entry#4 \
		-background #9d3d00 \
		-cursor {} \
		-foreground white \
		-highlightbackground #d88700 \
		-textvariable LoadFrom \
		-width 71
	catch {
		$base.entry#4 configure \
			-font -*-screen-Medium-R-Normal-*-*-140-*-*-*-*-*-*
	}

	label $base.label#7 \
		-background #d88700 \
		-highlightbackground #d88700 \
		-justify left \
		-text {Process ID }
	catch {
		$base.label#7 configure \
			-font -*-Helvetica-Bold-R-Normal-*-*-100-*-*-*-*-*-*
	}

	entry $base.entry#7 \
		-background #9d3d00 \
		-cursor {} \
		-foreground white \
		-highlightbackground #d88700 \
		-textvariable ProcessID \
		-width 16
	catch {
		$base.entry#7 configure \
			-font -*-screen-Medium-R-Normal-*-*-140-*-*-*-*-*-*
	}

	button $base.button#8 \
		-activebackground #ffa831 \
		-background #d88700 \
		-command {MsgStoreTo .} \
		-highlightbackground #d88700 \
		-justify left \
		-padx 10 \
		-pady 3 \
		-text Store
	catch {
		$base.button#8 configure \
			-font -*-Helvetica-Bold-R-Normal-*-*-100-*-*-*-*-*-*
	}

	entry $base.entry#6 \
		-background #9d3d00 \
		-cursor {} \
		-foreground white \
		-highlightbackground #d88700 \
		-textvariable StoreTo \
		-width 71
	catch {
		$base.entry#6 configure \
			-font -*-screen-Medium-R-Normal-*-*-140-*-*-*-*-*-*
	}

	button $base.button#4 \
		-activebackground #ffa831 \
		-activeforeground red \
		-background #d88700 \
		-command { MsgQuit .} \
		-foreground red \
		-highlightbackground #d88700 \
		-padx 10 \
		-pady 3 \
		-text {  Quit}
	catch {
		$base.button#4 configure \
			-font -*-Helvetica-Bold-R-Normal-*-*-100-*-*-*-*-*-*
	}


	# Geometry management

	blt_table $root $base.frame#1 	1,1  \
		-columnspan 18 \
		-fill both \
		-rowspan 12
	blt_table $base.frame#1 $base.message#1 	1,1  \
		-anchor sw \
		-columnspan 27 \
		-fill x
	blt_table $base.frame#1 $base.listbox#1 	2,1  \
		-columnspan 27 \
		-fill both \
		-rowspan 9
	blt_table $base.frame#1 $base.scrollbar#1 	2,28  \
		-fill y \
		-rowspan 9
	blt_table $base.frame#1 $base.label#1 	11,1  \
		-columnspan 3 \
		-fill x
	blt_table $base.frame#1 $base.entry#1 	11,4  \
		-columnspan 15 \
		-fill x
	blt_table $base.frame#1 $base.label#9 	11,20  \
		-columnspan 2 \
		-fill x
	blt_table $base.frame#1 $base.message#4 	11,22  \
		-columnspan 2 \
		-fill x
	blt_table $base.frame#1 $base.button#1 	11,25  \
		-columnspan 4 \
		-fill x
	blt_table $base.frame#1 $base.label#2 	12,1  \
		-columnspan 3 \
		-fill x
	blt_table $base.frame#1 $base.entry#2 	12,4  \
		-columnspan 2 \
		-fill x
	blt_table $base.frame#1 $base.checkbutton#1 	12,7  \
		-columnspan 4 \
		-fill x
	blt_table $base.frame#1 $base.button#5 	12,12  \
		-columnspan 3 \
		-fill x
	blt_table $base.frame#1 $base.message#2 	12,16  \
		-columnspan 3 \
		-fill x
	blt_table $base.frame#1 $base.button#10 	12,20  \
		-columnspan 4 \
		-fill x
	blt_table $base.frame#1 $base.button#3 	12,25  \
		-columnspan 4 \
		-fill x
	blt_table $base.frame#1 $base.label#3 	13,1  \
		-columnspan 3 \
		-fill x
	blt_table $base.frame#1 $base.entry#3 	13,4  \
		-columnspan 2 \
		-fill x
	blt_table $base.frame#1 $base.checkbutton#2 	13,7  \
		-columnspan 4 \
		-fill x
	blt_table $base.frame#1 $base.button#6 	13,12  \
		-columnspan 3 \
		-fill x
	blt_table $base.frame#1 $base.entry#10 	13,16  \
		-columnspan 3 \
		-fill x
	blt_table $base.frame#1 $base.checkbutton#4 	13,25  \
		-columnspan 4 \
		-fill x
	blt_table $base.frame#1 $base.label#5 	14,1  \
		-columnspan 3 \
		-fill x
	blt_table $base.frame#1 $base.entry#5 	14,4  \
		-columnspan 2 \
		-fill x
	blt_table $base.frame#1 $base.checkbutton#3 	14,7  \
		-columnspan 4 \
		-fill x
	blt_table $base.frame#1 $base.button#7 	14,12  \
		-columnspan 3 \
		-fill x
	blt_table $base.frame#1 $base.label#8 	14,25  \
		-columnspan 3 \
		-fill x
	blt_table $base.frame#1 $base.entry#9 	14,28  \
		-fill x
	blt_table $base.frame#1 $base.button#9 	15,1  \
		-columnspan 3 \
		-fill x
	blt_table $base.frame#1 $base.entry#4 	15,4  \
		-columnspan 15 \
		-fill x
	blt_table $base.frame#1 $base.label#7 	15,22  \
		-columnspan 3 \
		-fill x
	blt_table $base.frame#1 $base.entry#7 	15,25  \
		-columnspan 4 \
		-fill x
	blt_table $base.frame#1 $base.button#8 	16,1  \
		-columnspan 3 \
		-fill x
	blt_table $base.frame#1 $base.entry#6 	16,4  \
		-columnspan 15 \
		-fill x
	blt_table $base.frame#1 $base.button#4 	16,25  \
		-columnspan 4 \
		-fill x

	# Resize behavior management

	blt_table row $root configure all -resize none
	blt_table row $root configure 1 -height {30 Inf}
	blt_table row $root configure 2 -height {30 Inf}
	blt_table row $root configure 3 -height {30 Inf}
	blt_table row $root configure 4 -height {30 Inf}
	blt_table row $root configure 5 -height {30 Inf}
	blt_table row $root configure 6 -height {30 Inf}
	blt_table row $root configure 7 -height {30 Inf}
	blt_table row $root configure 8 -height {30 Inf}
	blt_table row $root configure 9 -height {30 Inf}
	blt_table row $root configure 10 -height {30 Inf}
	blt_table row $root configure 11 -height {30 Inf}
	blt_table row $root configure 12 -height {30 Inf}
	blt_table column $root configure all -resize none
	blt_table column $root configure 1 -width {30 Inf}
	blt_table column $root configure 2 -width {30 Inf}
	blt_table column $root configure 3 -width {30 Inf}
	blt_table column $root configure 4 -width {30 Inf}
	blt_table column $root configure 5 -width {30 Inf}
	blt_table column $root configure 6 -width {30 Inf}
	blt_table column $root configure 7 -width {30 Inf}
	blt_table column $root configure 8 -width {30 Inf}
	blt_table column $root configure 9 -width {30 Inf}
	blt_table column $root configure 10 -width {30 Inf}
	blt_table column $root configure 11 -width {30 Inf}
	blt_table column $root configure 12 -width {30 Inf}
	blt_table column $root configure 13 -width {30 Inf}
	blt_table column $root configure 14 -width {30 Inf}
	blt_table column $root configure 15 -width {30 Inf}
	blt_table column $root configure 16 -width {30 Inf}
	blt_table column $root configure 17 -width {30 Inf}
	blt_table column $root configure 18 -width {30 Inf}

	blt_table row $base.frame#1 configure all -resize none
	blt_table row $base.frame#1 configure 1 -height {30 Inf}
	blt_table row $base.frame#1 configure 2 -height {30 Inf}
	blt_table row $base.frame#1 configure 3 -height {30 Inf}
	blt_table row $base.frame#1 configure 4 -height {30 Inf}
	blt_table row $base.frame#1 configure 5 -height {30 Inf}
	blt_table row $base.frame#1 configure 6 -height {30 Inf}
	blt_table row $base.frame#1 configure 7 -height {30 Inf}
	blt_table row $base.frame#1 configure 8 -height {30 Inf}
	blt_table row $base.frame#1 configure 9 -height {30 Inf}
	blt_table row $base.frame#1 configure 10 -height {30 Inf}
	blt_table row $base.frame#1 configure 11 -height {30 Inf}
	blt_table row $base.frame#1 configure 12 -height {30 Inf}
	blt_table row $base.frame#1 configure 13 -height {30 Inf}
	blt_table row $base.frame#1 configure 14 -height {30 Inf}
	blt_table row $base.frame#1 configure 15 -height {30 Inf}
	blt_table row $base.frame#1 configure 16 -height {30 Inf}
	blt_table column $base.frame#1 configure all -resize none
	blt_table column $base.frame#1 configure 1 -width {30 Inf}
	blt_table column $base.frame#1 configure 2 -width {30 Inf}
	blt_table column $base.frame#1 configure 3 -width {30 Inf}
	blt_table column $base.frame#1 configure 4 -width {30 Inf}
	blt_table column $base.frame#1 configure 5 -width {30 Inf}
	blt_table column $base.frame#1 configure 6 -width {30 Inf}
	blt_table column $base.frame#1 configure 7 -width {30 Inf}
	blt_table column $base.frame#1 configure 8 -width {30 Inf}
	blt_table column $base.frame#1 configure 9 -width {30 Inf}
	blt_table column $base.frame#1 configure 10 -width {30 Inf}
	blt_table column $base.frame#1 configure 11 -width {30 Inf}
	blt_table column $base.frame#1 configure 12 -width {30 Inf}
	blt_table column $base.frame#1 configure 13 -width {30 Inf}
	blt_table column $base.frame#1 configure 14 -width {30 Inf}
	blt_table column $base.frame#1 configure 15 -width {30 Inf}
	blt_table column $base.frame#1 configure 16 -width {30 Inf}
	blt_table column $base.frame#1 configure 17 -width {30 Inf}
	blt_table column $base.frame#1 configure 18 -width {30 Inf}
	blt_table column $base.frame#1 configure 19 -width {30 Inf}
	blt_table column $base.frame#1 configure 20 -width {30 Inf}
	blt_table column $base.frame#1 configure 21 -width {30 Inf}
	blt_table column $base.frame#1 configure 22 -width {30 Inf}
	blt_table column $base.frame#1 configure 23 -width {30 Inf}
	blt_table column $base.frame#1 configure 24 -width {30 Inf}
	blt_table column $base.frame#1 configure 25 -width {30 Inf}
	blt_table column $base.frame#1 configure 26 -width {30 Inf}
	blt_table column $base.frame#1 configure 27 -width {30 Inf}
	blt_table column $base.frame#1 configure 28 -width {30 Inf}
# additional interface code
# end additional interface code

}

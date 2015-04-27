package SEQCrawlerParser;

# This package provides subroutines for parsing ioc startup
# command files. 
#

use FileHandle;
use File::Basename; 
use File::Compare;
use File::Path;
use File::stat;

require Exporter;
use POSIX;
use LogUtil;

our @ISA = qw(Exporter);
our @EXPORT = qw(parse_ld_seq parse_seq_command get_seq_hits @load_hits @seq_hits);
our @EXPORT_OK = qw();
our $VERSION = 1.00;

sub get_seq_hits {
   return \@seq_hits;
}

# **************************************************************************
# Subroutine: Parse up file loaded with VxWorks ld command (in a startup file)
#             Extract sequence programs loaded by that line
# **************************************************************************
#
# The caller passes us one ld cmd file from the startup file.
# This subroutine finds sequence programs loaded in that line (either inside a 
# library or as stand-alone object files
# Then the routine pushes the filename of the intermediate c program created by the snc
# (including path) as an element of an array of potential seq files to run


sub parse_ld_seq {

    my ($ld_cmd_file, $arch, $stcmd_dir) = @_;
    use Shell qw(find ls strings grep);          # these shell commands used below.

    @string_list_a = split ('/',$stcmd_dir);     # assuming st.cmd is two levels under top
    my $size = @string_list_a;                   # find top by removing the last two dirs
    if ($size > 0) {                             # on the $stcmd_dir path
        $remove = $string_list_a[$size - 2].'/'.$string_list_a[$size -1];
	$stcmd_dir =~ s/$remove//;
	$top_dir = $stcmd_dir;
    }  
   
    # form find arg string with grep cmd embedded on it, to get something of the form:
    #   > find /usr/local/iocapps/R3.13.10/ioc/linac/2/ | grep O.mv167/rfgtsLib
    # The result will be something of the form:
    # /usr/local/iocapps/R3.13.10/ioc/linac/2/rfgtsApp/src/O.mv167/rfgtsLib

    $find_cmd_arg = "$top_dir -name $ld_cmd_file | grep O.$arch";
    @app_lib_list_all = find ($find_cmd_arg);
    $size_all = @app_lib_list_all;        
    $file_found = 0;
    if ($size_all > 0) {
        $app_lib_path = dirname(@app_lib_list_all[0]).'/';
        $file_found = 1;
        if ($file_found) {
            if ($ld_cmd_file =~ /\.o$/) {                       
	    
	        # If loaded file is .o, we'll check whether it is a sequence program 
		# We'll look for a .c program in that same directory, assuming the snc 
		# has created one in that same location
		
	        $file_full_line = $app_lib_path.$ld_cmd_file;    
                $file_full_line =~ s/\.o/\.c/;                   
                # check whether .c program found was actually generated by snc
		# "extern seqProgram" line must appear inside the c file
		
		if (-f $file_full_line){
	            $grep_cmd = $file_full_line . ' | xargs grep seqProgram | grep extern';
		
	            $grep_cfile_list = ls ($grep_cmd);                 
	            @grep_cfile_results = split ('\n',$grep_cfile_list);
	            $size_grep = @grep_cfile_results;
	            if ($size_grep > 0)
	            {  
                        # check whether it actually got a file back
		
                        if ($grep_cfile_results[0] !~ m/^\s*No such file/i){   
                            log('debug', "SEQ_PV Sequence c file is: $file_full_line"); 
                            push (@load_hits, $file_full_line);
                        }
	            }
	        }
	        else {
	            log('debug', "SEQ_PV loaded file not not a sequence file"); 
	        }
	    }
            else {
	        # If loaded file has no .o extension, we'll check what c programs are in the same
		# directory, and whether they were included in that library
		# As a first approach, we'll look for seq programs (c programs, once compiled) in
		# the same area the library is

                $grep_cmd = $app_lib_path . '*.c';
                @grep_cfile_results = glob("$grep_cmd");
	        
		if (@grep_cfile_results){
		
                        foreach (@grep_cfile_results) {   
                            # check whether .c program found was actually generated by snc
                            # "extern seqProgram" line must appear inside the c file
	                    $grep_result_str = $_;
                            $grep_cmd_2 = $grep_result_str . ' | xargs grep seqProgram | grep extern';
                            @grep_cfile_results_2= ls ($grep_cmd_2);
                            $size_st_results_2 = @grep_cfile_results_2;

                            if ($size_st_results_2>0){

                                if ($grep_cfile_results_2[0] !~ m/^\s*No such file/i) {
                                    $file_full_line = $grep_result_str;	     
                                    $file = $grep_cfile_results_2[0];
                                    $file =~ s/extern struct seqProgram //;
                                    $file =~ s/\;//;
                                    $file =~ s/\n//;
                                    $strings_cmd = $app_lib_path .$ld_cmd_file. ' | grep '. $file;
                                    $lib_objs = strings ($strings_cmd);
                                    @lib_objs_list = split ('\n',$lib_objs);
                                    $size_lib_objs = @lib_objs_list;
                                    if ($size_lib_objs > 0) {
                                        $file_full_line =~ s/\.o/\.c/;
                                        log('debug', "SEQ_PV Sequence c file is: $file_full_line "); 
                                        push (@load_hits, $file_full_line);       # stick .c file into array of ld'd files.
	                            } else {
                                        if ($arch =~ m/^RTEMS/) { # strings doesnt work;assume file is included 
                                            $file_full_line =~ s/\.o/\.c/;
                                            log('debug', "SEQ_PV Sequence c file is: $file_full_line ");
                                            push (@load_hits, $file_full_line);
                                        } else {
                                            log('debug', "SEQ_PV seqProgram $file not found in loaded file $ld_cmd_file "); 
                                        }
				    }
                                } # end 'No such file' if

		            } 
			    
	                } # end foreach @grep_cfile_results
		    
	        } # end if $grep_cfile_results_size
		
            } # end else
        } else {
            log('debug', "SEQ_PV loaded file not not a sequence file"); 
        }
    } else {
        log('debug', "SEQ_PV loaded file not not a sequence file"); 
    }
    return 0;
}

# ******************************************************************************
# Subroutine: Parse Sequence command (seq command is in a startup related file).
# ******************************************************************************
#
# The caller passes us one seq line from a startup file.
# The ld_hits array contains the list of loaded .c files.
# 1) find the .o file which contains the entry point of this seq command.
#    NOTE: a given .o file may contain multiple entries.  So, if the file is already
#          in seq_hits, don't add it again.  In that case, just return.
# 2) find all the pv's in that .c file.  Store them in pv_list array.
# 3) Create a row in the seq_hits array for the file name, list of pvs, and IOC name.
#
# Arguments:
# 1. Script line (one line of startup file)
# 2. ioc name


sub parse_seq_command {

    my ($script, $ioc_nm) = @_;
    use Shell qw(strings grep ls);                    # these shell commands used below.

    log('info', "SEQ_PV seq command is $script"); 
    log('debug', "SEQ_PV IOC is: $ioc_nm");

    $script_line = $script;
    $script_line =~ s/^seq//;   # strip seq cmd (in case it's there - Epics 3.14)
    $script_line =~s/^\s+//;
    ($seqName_a,$seqName) = split (/,| /,$script_line,2);
    $seqName_a =~ s/^&//;
    $seqName='' if !$seqName;

#   Find all the environment variable in getenv statements on the SEQ line and store into genv_a.
#   Example seq(&seqName, getenv("DATABASE_MACROS"))
#
#   NOTE: Only 3 occurances of getenv are supported now.  
#         Cut/paste logic below to add more.
#
    undef(@genv_a);                                  # free previous storage.

    $genvIx = index($seqName,"getenv");
    if ($genvIx > 0) { 
        $genv = substr($seqName,$genvIx, 120);       # Take 120 chars after "getenv".
        chomp($genv);
        log('debug', "SEQ_PV  GETENV line is $genv");
        my @temp_a = split (/"/, $genv);   # "
        # print "ENV_VARB 1 is $temp_a[1] \n";
        push (@genv_a, $temp_a[1]);
                            
        $genvIx = index($genv,"getenv", 1);          # find next getenv
        if ($genvIx > 0) { 
            $genv = substr($genv,$genvIx, 120);      # Take 120 chars after "getenv".
            chomp($genv);
            # print "GETENV line is $genv\n";     
            @temp_a = split (/"/, $genv);   # "
            # print "ENV_VARB 2 is $temp_a[1] \n"; 
            push (@genv_a, $temp_a[1]);
            $genvIx = index($genv,"getenv", 1);      # find next getenv
	    if ($genvIx > 0) { 
                $genv = substr($genv,$genvIx, 120);  # Take 120 chars after "getenv".
                chomp($genv);
                # print "GETENV line is $genv\n";       
                @temp_a = split (/"/, $genv);   # "
                # print "ENV_VARB 3 is $temp_a[1] \n"; 
                push (@genv_a, $temp_a[1]);
            }
        }
    }   # end first genvIx
            
    my $number = @genv_a;
    # print ("GENV1 has $number elements and CONTAINS: \n"); 
    # foreach (@genv_a) {                                     
    #   my $name = $_;
    #   print "GENV1 ELEM is: $name \n";
    # }
            
    # Find the env variable assignments within sequence commands and store in varbHash.
    # They are surrounded by quotes with at least one equal sign inside.
    # Example: seq & power_proc,"name=pp1,S=TRS1"
            
    my @setenv_a = split (/"/, $seqName);          # " seqName is everything after &
                                  
    undef(%varbHash);                              # free up previous storage.
                                  
    foreach $quotElem (@setenv_a) {                # loop across entries delimited by quotes.
        $quotElem =~s/\s//g;
        if (index($quotElem,"=") > 0) {
            my @singleSetting_a = split (/,/, $quotElem);     # might be more than one setting inside quotes.
            foreach $commaElem (@singleSetting_a) {           # loop across individual a=b entries delim by commas
                if (index($commaElem,"=") > 0) {
                  my @varbSetting_a = split (/=/, $commaElem);  # get things on each side of equal sign.
                  $varbHash{$varbSetting_a[0]} = $varbSetting_a[1];
                }
            }
        }
    }  # end foreach quotElem.
                          
                          
#    if (defined(%varbHash)) {
#        log('debug', "SEQ_PV::111  Here is the environment varbHash ");
#        while (@temp=each(%varbHash)) {               
#            log('debug', "SEQ_PV::111  The key value pair is:   @temp")
#        } 
#    } else {
#        log('debug', "SEQ_PV::111 varbHash is empty due to no X=Y settings on seq line ");
#    }
                          
    # We created genv_a above.  For each genv entry, look it up in gbl_var.
    # If it's in gbl_var, the value in gbl_var may be one of these cases:
    #    1) Value
    #    2) X=Y
    #    3) X=Y A=B...
    # In case 1, we do nothing.  Maybe this never happens??
    # In case 2 and 3, we add the variable and value to #varbHash
                                  
    $number = @genv_a;
    if ($number > 0) {                                      # if there are any entries in genv_a
        foreach $genvElem (@genv_a) {           
            $genvVal = $gbl_var{$genvElem};
            log('debug', "SEQ_PV   key = $genvElem  value = $gbl_var{$genvElem}");

            if (!$genvVal) {
                log('warn', "SEQ_PV   $genvElem ** NOT ** found in gbl_var. Im ignoring it");
            }
            else {
                log('debug', "SEQ_PV   $genvElem = $genvVal in gbl_var ");
                #
                # Add the entry found in gbl_var (something like A=B) to the varHash.     
                #
                @setenv_a = split (/"/, $genvVal);          # " genvVal is something like A=B

                foreach $quotElem (@setenv_a) {             # loop across entries delimited by quotes.
                    log('debug', "SEQ_PV Quote delimited entry is $quotElem ");
                    if (index($quotElem,"=") > 0) {
                        @singleSetting_a = split (/,/, $quotElem);     # might be more than one setting inside quotes.
                        foreach $commaElem (@singleSetting_a) {        # loop across individual a=b entries delim by commas
                            @varbSetting_a = split (/=/, $commaElem);  # get things on each side of equal sign.
                            log('debug', "SEQ_PV VARB IS $varbSetting_a[0], VALUE IS $varbSetting_a[1] ");
                            $varbHash{$varbSetting_a[0]} = $varbSetting_a[1];
                        }
                    }
                }  # end foreach quotElem.            
            }    # end if genvVal found
        }    # end for each genv_a
    }   # end if entries in genv_a
        
                          
#    log('debug', "SEQ_PV:222  The input line is: $script_line ");
#    if (defined(%varbHash)) {
#        log('debug', "SEQ_PV::222  Here is the environment varbHash ");
#        while (@temp=each(%varbHash)) {               
#            log('debug', "SEQ_PV::222  The key value pair is:   @temp")
#        } 
#    }
#    else {
#        log('debug', "SEQ_PV::222 varbHash is empty due to no X=Y settings on seq line ");
#    }
        
    #   Loop across array of ld'd files: load_hits.
    #   See if seqName_a is in that file by using the strings command.
    #   If so, save the seqName_a in a local variable.
        
    my $seqFound = 0;

    foreach(@load_hits) {
        #
        #  Do the shell strings cmd to find which ld'd .o file has it's entry point .
        # 
        $fullFileName = $_;                # $_ is current array element. 
        $list = strings ($fullFileName);
       
        #  $list now has output of strings done on the file.o in a scalar.  
        #  So, let's look for our entry point.

        $grepStr = 'extern struct seqProgram '.$seqName_a.';';           
        log('debug', "SEQ_PV Searching for entry $seqName_a in strings of $fullFileName");
        if ($list =~ m/$grepStr/) {
            $dot_c_file = $fullFileName;      # this is the file containing the entry point.
            log('debug', "SEQ_PV Found entry $seqName_a in $fullFileName");
            $seqFound = 1;
            last;                   # break out of loop
	} 

    } # end for ld_files
        
    # We're all done so return if the entry point was not found in a .c file.
    if (!$seqFound) {
        log('error', "SEQ_PV  ERROR** Didnt find $seqName_a in any loaded files. IOC = $ioc_nm ");
        return;
    } 
        
    # string_list_a contains strings output from last .c file which is the 
    # one where we found the entry for the current sequence command.
    @string_list_a = split ('\n',$list);
        
    my @seq_pv_list = ();

    foreach(@string_list_a) {
        #  Do macro substution on the PV. 
        #  Note: gbl_var (environment) lookup is done elsewhere already 
        #        so we don't need to look there now.
       
        # MODIFY THIS REGEX FOR EACH SITE.  IT IS THE FORMAT OF A PV.
        my $pvNameStr = $_;
        if ($pvNameStr =~ m/\"\,\ \(void\ \*\)/)
        { 
            $bracket_1 = index($pvNameStr, '"');
            $bracket_2 = index($pvNameStr, '"',$bracket_1 + 1 );
            $pvName = substr ($pvNameStr, $bracket_1+1, $bracket_2-$bracket_1-1);
            if (index($pvName,"{") >= 0) {           # if there is a macro of the form {MACRO} 
                LogUtil::log('debug', "SEQ_PV Doing macro replacement on $pvNameStr \n");
                @bracket_a = split("{",$pvName);    # array contains the individual macros like MAC}xxx
                # $number_xxx = @bracket_a;                                               #DEBUG
                # print "SEQ found *** $number_xxx *** number entries in bracket_a \n";   #DEBUG
                # Remember this perl rule for the split function!
                # If the delimiter is at the start of the string then the first 
                # element in the array of results will be empty.
                # SO, @bracket_a might have an empty first element!!!
                my $ii = 0;
                foreach my $pvMacro (@bracket_a) {
                    $ii++;
                    if ($ii == 1) {next;}                                      # first entry stuff before {
                    unless ($pvMacro eq "") {                                  # if element not empty
                        #LogUtil::log('debug', "SEQ_PV After left bracket array entry is $pvMacro \n");
                        @pvMacr_a = split("}",$pvMacro);                       # capture just the macro
                        $pvMacro = $pvMacr_a[0];
                        #LogUtil::log('debug', "SEQ_PV pvMacro found = $pvMacro \n");
                        if ((exists $varbHash{$pvMacro}) && defined $varbHash{$pvMacro} ) {
                           #LogUtil::log('debug', "SEQ_PV Hash value for $pvMacro is $varbHash{$pvMacro} \n");
                            $pvName =~ s/{$pvMacro}/$varbHash{$pvMacro}/;   #substutute {MACRO} from value in $varbHash
                            $macAdd = "'". $pvMacro . "=".$varbHash{$pvMacro}."' ";
			    if ($dot_c_file !~ m/$macAdd/){
			        $dot_c_file = $macAdd.$dot_c_file;
			    }
                        }
                        # If Macro is not in varbHash, then it wasn't set by getenv or X=Y on sequence line.
                        # So, let's look in gbl_var (the raw set of environment variables set by pv parser).
                        else {                                
                            if (exists $gbl_var{$pvMacro}) {
                                $pvName =~ s/{$pvMacro}/$gbl_var{$pvMacro}/;  #substutute {MACRO} from value in $gbl_var
				$macAdd = "'". $pvMacro . "=".$gbl_var{$pvMacro}."' ";
				if ($dot_c_file !~ m/$macAdd/){
				    $dot_c_file = $macAdd.$dot_c_file;
				}
				
				#LogUtil::log('debug', "SEQ_PV gbl_var value for $pvMacro is $gbl_var{$pvMacro}  \n");
                            }
                            else {

                                # Look at output of strings cmd done on .o file in scalar $list here for 
                                # $pvMacro=XXX and use XXX as the value in the pv name.
                                # It's in the .o file if the .o file's source code has an env varb 
                                # setting in the program stmnt.
                                # Note: on my solaris machine, the first line of output is: 4STN=LB60 where
                                # I assume the number 4 is the strings argument -number (4 is default).
                                # This might require SITE MODIFICATION depending on the compiler used.

                                if ($list =~ /$pvMacro\s*=\s*([\w\:]+)/  || 
                                    $list =~ /\s*$pvMacro\s*=\s*([\w\:]+)/ ) {   # paren thingie is put into $1
                                    #LogUtil::log('debug', "SEQ_PV strings value for $pvMacro is $1 \n");
                                    my $macValue = $1;
                                    $pvName =~ s/{$pvMacro}/$macValue/;
				    $macAdd = "'". $pvMacro . "=".$macValue."' ";
				    if ($dot_c_file !~ m/$macAdd/){
				        $dot_c_file = $macAdd.$dot_c_file;
				    }
                                }
                                else {
                                    LogUtil::log('warn', 
                                        "SEQ_PV  WARNING! Macro $pvMacro not found in varbHash, glb_var, or .o strings ");
                                }
                            }
                        }
                    }
                }    # end for
                LogUtil::log('debug', "SEQ_PV  Sequence macro substitution complete.  Result is $pvName");
            } 
#           if ($pvName =~ m/\w:\w/ && $pvName =~ m/^[a-zA-Z]/ && $pvName !~ m/[\s\=]/) {           # if pvName is pv
            push (@seq_pv_list,$pvName);        # save PV for use below
#           }
	}
        
    }   # end for each string_list
        
    # Construct seq_hits array for use by SEQCrawlerDB which will store into DB.
    # One entry of seq_hits contains these:
    # 1) .o file name including path
    # 2)  array of PV's in that file.
    # 3) IOC name.
    
    $dot_c_file = $fullFileName;      # this is the file containing the entry point.
    $dot_st_file = $dot_c_file;
    $dot_st_file =~ s/O\..*\///;      # remove O.<arch>/
    $dot_st_file =~ s/\.c/\.st/;      # change filename suffix to .st
   
    if (! -f $dot_st_file ) {
        $dot_st_file =~ s/\.st/\.stt/;
    }
    if ( -f $dot_st_file ) {
        $rel_info = $ioc_nm." ".$script;
        push (@seq_hits, [$dot_st_file, \@seq_pv_list, $rel_info]);
        log('info',"SEQ_PV Sequence source file is $dot_st_file");
        $pv_count=scalar(@seq_pv_list);
        log('info',"SEQ_PV Number of sequencer pvs is  $pv_count");
        log('debug',"SEQ_PV Sequencer pvs are  @seq_pv_list");
        log('debug',"SEQ_PV rel_info is $rel_info");
    }
    else {
        log('error',"SEQ_PV sequencer source file for $fullFileName NOT FOUND");
    }

    return;
        
}   # end of parse_seq_command subr
                          

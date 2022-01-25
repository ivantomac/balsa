#! /bin/sh
exec perl -x $0 ${1+"$@"}
    if 0;
#!perl

# Fixup possible bugs in KCT in Semantics.c
# Remove repeated insertion of # line delimiter code blocks

%blocks = ();

$currentBlockLineNo = -1;
@currentBlockLines = ();
@repeatBlockLines = ();
$prevLine = "";

while (<>)
{
	$line = $_;

	if ($line =~ /^yyVisit/ and $line eq $prevLine)
	{
		# Skip repeated yyVisit lines
	} elsif ($line =~ /^case (k.*):/)
	{
		ConsiderEndOfBlock ();
		print $line;
	} elsif ($line =~ /^\# line (\d+)/)
	{
		ConsiderEndOfBlock ();
		$currentBlockLineNo = $1;
		if (defined $blocks{$currentBlockLineNo})
		{
			@repeatBlockLines = @{$blocks{$currentBlockLineNo}};
		} else {
			@repeatBlockLines = ();
		}
		print $line;
	} else {
		push (@currentBlockLines, $line);
		if ($#repeatBlockLines != -1)
		{
			if ($repeatBlockLines[0] ne $line)
			{
				print $line;
				@repeatBlockLines = ();
			}
			shift @repeatBlockLines;
		} else {
			print $line;
			@repeatBlockLines = ();
		}
	}

	$prevLine = $line;
}

sub ConsiderEndOfBlock
{
	if ($currentBlockLineNo != -1)
	{
		$blocks{$currentBlockLineNo} = [@currentBlockLines];
		$isNewBlock = 0;
	}
	@currentBlockLines = ();
}

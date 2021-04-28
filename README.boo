Boo's translation notes
Rev. 2002.10.03
-----------------------

1. License
2. Intro
3. To do
4. After Compiling
5. Testing Text


1. License
This project has been GPL'd. See LICENSE for full details.


2. Intro
Hello!

First check out README.jay for info not contained in this file regarding translating text.
All the English translations I've finished to date are in the /script directory.  You'll also want to grab  'Boo's Reference Materials' under the 'Downloads' section from the main webpage (fe4trans.freeshell.org).  If you notice the 'Script Dump Progress' (on the main page), all of the dumped scripts are finished.  I have, however, felt it necessary to go back and revise what I've done in order to make the English more consistant throughout.  At this time I've finished revising chap. 0, 1 and half of 2.  I probably should be the one to finish the revision for consistency unless you've gone through *everything* and feel comfortable you can take it on.  If you want to translate something, work on the missing dumps (see #3 below).  Most of the missing text has been located, but it needs to be redumped to bring it into more cohesive script chunks (Note: read #4 below for more info on this).


3. To do
The area that needs the most work is the ending dialog.  Check my reference materials under /reference/ending & /reference/»ö¤ÎÍÕ/Ending for some web sites containing the Japanese ending dialog.  Also, I found a lot of the ending dialog in Jay's missing dumps.  It just needs to be sorted out a bit.  The ending dialog is split into the character dialogs & the narrative dialog.  Anyone who feels comfortable taking this on (translation, working w/hacker on missing dumps to tidy them up, etc.) is more than welcome.
Make sure you check the README in the /missing directory (reference materials) to see what else I've already finished.  Also, the skill descriptions I haven't touched. 


4. After Compiling (read README.jay for compiling instructions)
Once you've compiled Jay's code, you can type 'make ips' to make a new patch with any new translations you've done.

'make dump' will extract the original Japanese dumps from the rom into the /fe4d/script directory for referencing.

'make missing' will extract missing scripts into the /fe4d/script directory with file names like 'fe4_missing0x.txt'.  These files are to be translated, and placed into the /script directory.  Then 'make ips' to see if English shows up where you think it should.  However, these dumps are still in a rough form as they contain lots of script that has already been dumped/translated mixed in with the new bits.  See 'missing/missing_notes/README' in my reference materials for my notes on their content.  Take a stab at bringing them into cohesive units with a hacker.  Check out the 'Status of scripts to be dumped' on the main page to see what were looking for in cohesive units.


5. Testing Text
I needed a way to test text wrapping, etc. in the game.  It's impossible to have game saves for every moment in the game, but I do have save states for the beginning of each chapter (my save states are located in 'Boo's Reference Materials' off the main page).
A quick and dirty way to edit/translate English text and see it in the game is to edit a file from the /script directory (or copy a '/fe4d/script/fe4_missing0x.txt' file into the /script directory and translate it) and temporarily paste the chunk of text you want to see to the top of the file (essentially putting this chunk at the beginning of the chapter) so you can see the text when you start up the appropriate chapter.

Confusing, yes.  So here's an example:
Open 'script/fe4_05.txt' and pretend we're editing the second chunk of script beginning at '# 0x30870c:'  I'm talking about the chunk where the first English is "Langobalt, I spotted Lord Vylon due west of here."  Go ahead and change some English around to see if it'll come up when we test it later on down.

Essentially, we are going to copy & paste this chunk in front of the first chunk of text so it'll (temporarily) replace the first chunk and will become the first text we see when chapter 5 is loaded up.  This method works great because even the proper characters will come up.

Ok.  After you've changed some English around, take the entire section of text beginning with '# 0x30870c:' and ending with '[exit]' and paste it just after the header (Note: the 3 lines located just after the GPL info at the very top make up the header).

(Start of Header)
# Fire Emblem 4 Script Dump
# ppt\fe4_05.ppt

# Filesize: 4194816 bytes
(End of Header)

(paste chunk here)


Now save the file, 'make ips' and make sure you have a save state for chap. 5 (found in /saves directory of 'Boo's Reference Materials').  This newly inserted text will be the first dialog to start the chapter after the chapter narration section has finished (can bypass the narration by hitting <enter> under ZSNES).  Once you've verified the text is wrapping the way you want, looks good, etc., copy it back to its original location.


Good luck!

Boo
2002.10.03

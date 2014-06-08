
                GNU Privacy Guard Plug-in for Becky! 2

                              Copyright (C) 2000-2004 Yasuhiro ARAKAWA
                                                       Version 1.0.5b2
                                                     November 12, 2004


About "GNU Privacy Guard Plug-in"

  "GNU Privacy Guard Plug-in" (BkGnuPG) is the plug-in software to be
  used with "Becky! Internet Mail Ver. 2" (B2).
  By using "GNU Privacy Guard" (GnuPG) you can sign or encrypt your
  e-mail message.  Also it can verify or decrypt incoming e-mail that
  was encrypted or signed by "Pretty Good Privacy" (PGP) or GnuPG.

  There is a "Becky PGP plug-in" (BkPGP) that comes with B2, but almost
  everything you can do through PGP can be done by GnuPG, and GnuPG
  supports functions not yet supported by PGP.


Environment

  Besides I am using confirmation only in the following platform,
  because there is not developmental environment, although I think that
  it has no problem if B2 and Win32 version GnuPG are using.

     Windows 2000 Professional Service Pack 3 (Japanese version)


Installation and uninstallation method

  I explain for the one who was downloaded a full package version
  in this chapter.
  The one who was downloaded a package of with the installer starts
  the file where you downloaded and shall comply with the instructions
  of the window screen.  Uninstall please do uninstallation from
  "Add/RemovePrograms" applet of control panel.

  Please confirm that the installation of B2 and GnuPG are completing
  first of all.  I recommend that does the following setting about GnuPG,
  especially.

     - PATH is set up to the folder that module (gpg.exe) is implementation.
     - GnuPG home directory is set up.

  Please see the README.W32 file that comes in the GnuPG zip file
  about these setting method.

  When the file where I downloaded is extracted the following file is
  able to take out it.

     - BkGnuPG load module (DLL file)
     - BkGnuPG source code file (zip compression)
     - Manual (the HTML file below "manual\" folder)
     - This file

  You copy BkGnuPG.dll of these files to the "PlugIns" folder that is
  in a B2 installation folder or data folder, to install BkGnuPG.

  The uninstallation is completion if you delete the DLL file where
  copied.  Please delete even this, if there is BkGnuPG.ini.  


Changelog

  November 12, 2004 Verion 1.0.5b2
    - Fixed the problem that PGP/MIME-part of original message is lost
      when moves or copies decryption message.

  May 03, 2003 Verion 1.0.5b
    - It can be Setting message digest algorithm to INI file.  Please
      refer to "manual\bkgnupg_ini_en.txt" for details.

  May 02, 2003 Verion 1.0.4
    - Modified Russian resources.
    - Changed font name "MS Shell Dlg" about English resources.

  May 01, 2003 Verion 1.0.4b (Beta version)
    - Added Russian resources.
    - Got the "gpg.exe" path from registory. (But the INI file is
      priority more.)
    - Support --use-agent option. (use it with "gpg-agent.exe" that is
      included to WinPT.)

  December 31, 2002 Verion 1.0.3
    - Fixed the problem that "gpg.exe" was hung up at the Encrypting.

  March 21, 2002 Verion 1.0.2
    - Fixed the duplication of header elements (ex. Content-Type)
      at the Decrypting.

  June 17, 2001 Verion 1.0.1
    - The problem is modified that "gpg.exe" did not trigger in the
      path including space code(s).
    - It corresponded to PGP/MIME signature in the text mode.
      (for Sylpheed)  Please refer to "manual\bkgnupg_ini_en.txt"
      for details.
    - It corresponded to the display of the passphrase for "Passphrase
      dialog".
    - It corresponded to UTF-8 charactor set for "Output dialog".

  November 08, 2000 Verion 1.0.0
    - Official version!!  Version notation is changed.
    - I start the distribution of the package with the installer.

  October 31, 2000 Verion 0.40
    - Include From user and default user to the recipients list.
    - The manual is modified.  Even the English manual prepared it.
    - It corresponded to SDK 0.33.

  October 23, 2000 Verion 0.32
    - I translate a part of the documents into English.  Yet, it may be
      strange expression because I am using translation software.
    - Several compiling option are changed.
    - The method of the processing that a file is deleted tried to be
      changed.  
    - BkGnuPG holds the menu ID that it registered in the inside.
    - The menu item of the List View is set in the upper side more
      "mail property".


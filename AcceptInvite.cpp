	/*
	* Copyright (C) 2004-2019 ZNC, see the NOTICE file for details.
	*
	* Licensed under the Apache License, Version 2.0 (the "License");
	* you may not use this file except in compliance with the License.
	* You may obtain a copy of the License at
	*
	*     http://www.apache.org/licenses/LICENSE-2.0
	*
	* Unless required by applicable law or agreed to in writing, software
	* distributed under the License is distributed on an "AS IS" BASIS,
	* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	* See the License for the specific language governing permissions and
	* limitations under the License.
	* 
	* 
	* ==================================================
	*  AutoInvite module by QueenElsa • Version 1.0
	* --------------------------------------------------
	*  Designed for use on the Undernet IRC Network
	* --------------------------------------------------
	*  Channel: #development @ irc.undernet.org
	*  Contact: QueenElsa@undernet.org
	* --------------------------------------------------
	*  Automatically accept invites from secure network 
	*  services. GNUWorld only. Does not support Anope/
	*  Atheme/Srvx services.
	* ==================================================
	*/


#include <znc/IRCNetwork.h>
#include <znc/znc.h>
#include <znc/Chan.h>
#include <znc/Modules.h>
#include <znc/Client.h>

using std::set;

class CAcceptInvite : public CModule {
protected:
    CString m_sCIdent;
    CString m_sCHost;
    CString m_sDIdent;
    CString m_sDHost;
    CString m_sEUIdent;
    CString m_sEUHost;
	CString m_sXIdent;
    CString m_sXHost;

	CString m_sCustNick;
    CString m_sCustIdent;
    CString m_sCustHost;
	CString m_sNetName;

    bool m_bOVR{};
    bool m_bAJC{};
    bool m_bAJD{};
	bool m_bAJX{};
	bool m_bAJEU{};
	bool m_bCUST{};
	bool m_bCBSU{};

	void SaveSettings() {
		SetNV("CIdent", m_sCIdent); //C's Ident
        SetNV("CHost", m_sCHost); //C's Host
		SetNV("DIdent", m_sDIdent); //D's Ident
        SetNV("DHost", m_sDHost); //D's Host
		SetNV("EUIdent", m_sEUIdent); //EUWorld's Ident
        SetNV("EUHost", m_sEUHost); //EUWorld's Host
		SetNV("XIdent", m_sXIdent); //X's Ident
        SetNV("XHost", m_sXHost); //X's Host
		SetNV("NetName", m_sNetName); //Network Name

        SetNV("Override", m_bOVR ? "0" : "1");
		SetNV("C", m_bAJC ? "1" : "0");
		SetNV("D", m_bAJD ? "1" : "0");
		SetNV("X", m_bAJX ? "1" : "0");
		SetNV("EU", m_bAJEU ? "1" : "0");
		SetNV("Cust", m_bCUST ? "1" : "0"); //Accept Custom Bot Invites
		SetNV("CBSU", m_bCBSU ? "0" : "1"); //Custom Bot Setup Flag
	}

public:
	MODCONSTRUCTOR(CAcceptInvite) {
		AddHelpCommand();
		AddCommand("About", static_cast<CModCommand::ModCmdFunc>(&CAcceptInvite::AboutCommand), "", "Show information about this module. If you've asked for help, you will need to give the information provided from this command.");
        AddCommand("AcceptInvites-C", static_cast<CModCommand::ModCmdFunc>(&CAcceptInvite::AcceptCCommand), "<on|off>", "When this is turned on, you will automatically accept all invites from C. If this option is turned off, you will not join any channels when invited by C.");
		AddCommand("AcceptInvites-D", static_cast<CModCommand::ModCmdFunc>(&CAcceptInvite::AcceptDCommand), "<on|off", "When this is turned on, you will automatically accept all invites from the Security Services. If this option is turned off, you will not join any channels when invited by the Security Services. This setting has no effect if you are not an IRC Operator. This setting also has no effect if you have no access to the Security Services.");
        AddCommand("AcceptInvites-EUWorld", static_cast<CModCommand::ModCmdFunc>(&CAcceptInvite::AcceptOperCommand), "<on|off>", "When this is turned on, you will automatically accept all invites from EUWorld. If this option is turned off, you will not join any channels when invited by EUWorld. This setting has no effect if you are not an IRC Operator.");
        AddCommand("AcceptInvites-X", static_cast<CModCommand::ModCmdFunc>(&CAcceptInvite::AcceptXCommand), "<on|off>", "When this is turned on, you will automatically accept all invites from X. If this option is turned off, you will not join any channels when invited by X.");
		AddCommand("Join", static_cast<CModCommand::ModCmdFunc>(&CAcceptInvite::OnJoinCommand), "<#channel>", "If you received an ignored invite, typing JOIN <#channel> in this window will accept the invite and join your client to the invited channel. If you wish to use this command with multiple channels at once, separate each channel with a comma. Do not use spaces.");
        AddCommand("NoJoin", static_cast<CModCommand::ModCmdFunc>(&CAcceptInvite::NoJoinCommand), "<on|off>", "When this command is turned on, you will not accept any invites. This setting overrides all other invite settings.");
		AddCommand("Setup", static_cast<CModCommand::ModCmdFunc>(&CAcceptInvite::SetupCommand), "", "Setup the module. This command will allow you to change the ident and host of the network services. This is useful if you're not an Undernet user. To get started with this command, just issue it with no parameters.");
		AddCommand("Show", static_cast<CModCommand::ModCmdFunc>(&CAcceptInvite::OnShowCommand), "", "Show the current settings for this module.");
	}

	bool OnLoad(const CString& sArgs, CString& sMessage) override {
		//Get Network Name
        m_sNetName = (GetNV("NetName"));
		if (m_sNetName.empty()) {
			m_sNetName = "UnderNET";
        }
        m_sCHost = (GetNV("CHost"));
		if (m_sCHost.empty()) {
			m_sCHost = "undernet.org";
        }

		//Get Chanfix
        m_sCIdent = (GetNV("CIdent"));
		if (m_sCIdent.empty()) {
			m_sCIdent = "chanfix";
        }
        m_sCHost = (GetNV("CHost"));
		if (m_sCHost.empty()) {
			m_sCHost = "undernet.org";
        }

		//Get Dronescan
        m_sDIdent = (GetNV("DIdent"));
		if (m_sDIdent.empty()) {
			m_sDIdent = "dronescan";
		}
        m_sDHost = (GetNV("DHost"));
		if (m_sDHost.empty()) {
			m_sDHost = "undernet.org";
        }

		//Get Euworld
        m_sEUIdent = (GetNV("EUIdent"));
		if (m_sEUIdent.empty()) {
			m_sEUIdent = "euworld";
		}
        m_sEUHost = (GetNV("EUHost"));
		if (m_sEUHost.empty()) {
			m_sEUHost = "undernet.org";
        }

		//Get X
        m_sXIdent = (GetNV("XIdent"));
		if (m_sXIdent.empty()) {
			m_sXIdent = "cservice";
		}
        m_sXHost = (GetNV("XHost"));
		if (m_sXHost.empty()) {
			m_sXHost = "undernet.org";
        }

		
		//Get X
        m_sCustNick = (GetNV("CustNick"));
		if (m_sCustNick.empty()) {
			m_sCustNick = "\0035!!NOT SET!!\003";
		}
        m_sCustIdent = (GetNV("CustIdent"));
		if (m_sCustIdent.empty()) {
			m_sCustIdent = "\0035!!NOT SET!!\003";
        }
		m_sCustHost = (GetNV("CustHost"));
		if (m_sCustHost.empty()) {
			m_sCustHost = "\0035!!NOT SET!!\003";
		}


		//Get flags
		m_bOVR = (GetNV("Override") == "0"); //Turned off by default. This setting should **NEVER** be enabled by default.

        m_bAJC = (GetNV("C") == "1"); //Turned on by default
        m_bAJD = (GetNV("D") == "0"); //Turned off by default
		m_bAJX = (GetNV("X") == "1"); //Turned on by default
		m_bAJEU = (GetNV("EU") == "0"); //Turned off by default
		m_bCBSU = (GetNV("CBSU") == "0"); //Turned off by default

		return true;
	}

	virtual EModRet OnInvite(const CNick& Nick, const CString& sChan) override {
		//Stop if NOJOIN is set.
		if(m_bOVR == true) { 
			PutModule("\x02\0034[INVITE REJECTED]\x02\003 The NOJOIN setting is turned on. The invite to " + sChan + " has been blocked.");
			return CONTINUE;
		}
		else if (m_bOVR == false) {
			//Handle ChanFix
			if ((Nick.GetIdent() == m_sCIdent) && (Nick.GetHost() == m_sCHost)) { 
				if (m_bAJC == true) {
					PutModule("\x02\00311 [AUTO-JOINED (ChanFix)] \x02\003 Official " + CString(m_sNetName) + " Service, " + Nick.GetNick() + " ("+ Nick.GetIdent() + "!" + Nick.GetHost() +") invited you to: " + sChan + ", automatically joining."); 
					PutIRC("JOIN " + sChan);
					return CONTINUE; 
				}
				else {
					PutModule("\x02\0034[INVITE REJECTED (ChanFix)]\x02\003 - The invite to " + sChan + " has been blocked as your current settings do not allow this module to accept invites from ChanFix.");
					return CONTINUE;
				}
			}
			//Handle EUWorld
			else if ((Nick.GetIdent() == m_sEUIdent) && (Nick.GetHost() == m_sEUHost)) { 
				if (m_bAJEU == true) {
					PutModule("\x02\0036 [AUTO-JOINED (Operator Service)] \x02\003 Official " + CString(m_sNetName) + " Service, " + Nick.GetNick() + " ("+ Nick.GetIdent() + "!" + Nick.GetHost() +") invited you to: " + sChan + ", automatically joining."); 
					PutIRC("JOIN " + sChan);
					return CONTINUE; 
				}
				else {
					PutModule("\x02\0034[INVITE REJECTED (Operator Service)]\x02\003 - The invite to " + sChan + " has been blocked as your current settings do not allow this module to accept invites from the Operator Service.");
					return CONTINUE;
				}
			}

			//Handle DroneScan
			else if ((Nick.GetIdent() == m_sDIdent) && (Nick.GetHost() == m_sDHost)) { 
				if (m_bAJD == true) {
					PutModule("\x02\0037 [AUTO-JOINED (Security Services)] \x02\003 Official " + CString(m_sNetName) + " Service, " + Nick.GetNick() + " ("+ Nick.GetIdent() + "!" + Nick.GetHost() +") invited you to: " + sChan + ", automatically joining."); 
					PutIRC("JOIN " + sChan);
					return CONTINUE; 
				}
				else {
					PutModule("\x02\0034[INVITE REJECTED (Security Services)]\x02\003 - The invite to " + sChan + " has been blocked as your current settings do not allow this module to accept invites from the Security Services.");
					return CONTINUE;
				}
			}

			//Handle CService
			else if ((Nick.GetIdent() == m_sXIdent) && (Nick.GetHost() == m_sXHost)) { 
				if (m_bAJX == true) {
					PutModule("\x02\0032 [AUTO-JOINED (Channel Services)] \x02\003 Official " + CString(m_sNetName) + " Service, " + Nick.GetNick() + " ("+ Nick.GetIdent() + "!" + Nick.GetHost() +") invited you to: " + sChan + ", automatically joining."); 
					PutIRC("JOIN " + sChan);
					return CONTINUE; 
				}
				else {
					PutModule("\x02\0034[INVITE REJECTED (Channel Services)]\x02\003 - The invite to " + sChan + " has been blocked as your current settings do not allow this module to accept invites from Channel Services.");
					return CONTINUE;
				}
			}

			//Handle Custom Defined Bot
			else if (m_bCBSU == true) {
				if (m_bCUST == true) {
					if ((Nick.GetNick() == m_sCustNick) && (Nick.GetIdent() == m_sCustIdent) && (Nick.GetHost() == m_sCustHost)) {
						PutModule("\x02\0039 [AUTO-JOINED (Custom Bot: " + CString(m_sCustNick) + ")\x02\003 - Your custom defined bot has invited you to: " + sChan + ", automatically joining.");
					}
				}
				if (m_bCUST == false) {
					PutModule("\x02\0034 [INVITE REJECTED (Custom Bot: " + CString(m_sCustNick) + ")\x02\003 - The invite to " + sChan + " has been blocked as your current settings do not allow this module to accept invtes from your custom defined bot.");
				}
			}

			//Handle Everyone Else
			else { 
			PutModule("\x02\0035 [INVITE IGNORED (Untrusted User)] \x02\003 " + Nick.GetNick() + " ("+ Nick.GetIdent() + "!" + Nick.GetHost() +") invited you to: " + sChan + " -- To accept this invite, type: join " + sChan);
			return HALT;
			}
		}
		else { return HALT; }
	}

    void OnJoinCommand(const CString& sCommand) {
        const CString& sArg = sCommand.Token(1,true);
        if(sCommand.Token(1).empty()) { PutModule("Syntax: JOIN <#channel[,#channel2,#channel3]>");}
            else {
                PutModule("\x02\00312 [MANUALLY JOINED (Override)] \x02\003 " + sCommand.Token(1));
                PutIRC("JOIN " + sCommand.Token(1));
            }
    }

    void NoJoinCommand(const CString& sCommand) {
        const CString& sArg = sCommand.Token(1,true);

        if (sArg.empty()) {
			PutModule("Syntax: NoJoin <on|off>");
			return;
		}
		if (sArg == "off") {
			PutModule("\x02[NOJOIN]\x02 You will accept invites based on the settings you defined.");
		}
		if (sArg == "on") {
			PutModule("\x02[NOJOIN]\x02 You will not accept any invites.");
            PutModule("\x02\0037WARNING:\x02\003 This setting overrides all other settings. This means, you won't accept invites from services like X while this setting is turned on. This module will inform you of each invite received.");
		}
		m_bOVR = sArg.ToBool();
		SaveSettings();
    }

    void AcceptCCommand(const CString& sCommand) {
		const CString& sArg = sCommand.Token(1, true);

		if (sArg.empty()) {
			PutModule("Syntax: AcceptInvites-C <on|off>");
			return;
		}
		if (sArg == "off") {
			PutModule("\x02[AcceptInvites-C]\x02 You won't accept invites from C anymore. You will need to manually join channels when invited by C.");
		}
		if (sArg == "on") {
			PutModule("\x02[AcceptInvites-C]\x02 You will accept invites from C again. Your client will automatically join any channel C invites you to.");
		}
		m_bAJC = sArg.ToBool();
		SaveSettings();
	}


    void AcceptDCommand(const CString& sCommand) {
		const CString& sArg = sCommand.Token(1, true);

		if (sArg.empty()) {
			PutModule("Syntax: AcceptInvites-D <on|off>");
			return;
		}
		if (sArg == "off") {
			PutModule("\x02[AcceptInvites-D]\x02 You won't accept invites from the Security Services anymore. You will need to manually join channels when invited by Security Services.");
		}
		if (sArg == "on") {
			PutModule("\x02[AcceptInvites-D]\x02 You will accept invites from the Security Services again. Your client will automatically join any channel that the Security Services invites you to.");
		}
		m_bAJD = sArg.ToBool();
		SaveSettings();
	}

    void AcceptOperCommand(const CString& sCommand) {
		const CString& sArg = sCommand.Token(1, true);

		if (sArg.empty()) {
			PutModule("Syntax: AcceptInvites-EUWorld <on|off>");
			return;
		}
		if (sArg == "off") {
			PutModule("\x02[AcceptInvites-EUWorld]\x02 You won't accept invites from EUWorld anymore. You will need to manually join channels when invited by EUWorld.");
		}
		if (sArg == "on") {
			PutModule("\x02[AcceptInvites-EUWorld]\x02 You will accept invites from EUWorld again. Your client will automatically join any channel EUWorld invites you to.");
		}
		m_bAJEU = sArg.ToBool();
		SaveSettings();
	}

	void AcceptXCommand(const CString& sCommand) {
		const CString& sArg = sCommand.Token(1, true);

		if (sArg.empty()) {
			PutModule("Syntax: AcceptInvites-X <on|off>");
			return;
		}
		if (sArg == "off") {
			PutModule("\x02[AcceptInvites-X]\x02 You won't accept invites from X anymore. You will need to manually join channels when invited by X.");
		}
		if (sArg == "on") {
			PutModule("\x02[AcceptInvites-X]\x02 You will accept invites from X again. Your client will automatically join any channel X invites you to.");
		}
		m_bAJX = sArg.ToBool();
		SaveSettings();
	}

    void AcceptCustomCommand(const CString& sCommand) {
		const CString& sArg = sCommand.Token(1, true);
		if (m_bCBSU == false) { 
			PutModule("\0034\x02 ERROR:\003\x02 You haven't defined a custom bot hostmask to accept invites from. To configure this setting, first use the command: SETUP CUSTOM.");
		}
		else if (m_bCBSU == true) {
			if (sArg.empty()) {
				PutModule("Syntax: AcceptCustom <on|off>");
				return;
			}
			if (sArg == "off") {
				PutModule("\x02[AcceptCustom]\x02 You won't accept invites from your custom defined bot anymore. You will need to manually join channels when invited by the bot.");
			}
			if (sArg == "on") {
				PutModule("\x02[AcceptCustom]\x02 You will accept invites from your custom defined bot again. Your client will automatically join any channel the bot invites you to.");
			}
			m_bCUST = sArg.ToBool();
			SaveSettings();
		}
	}

    void AboutCommand(const CString& sCommand) {
        PutModule("You are using version 3 of the AutoInvite module by QueenElsa.");
        PutModule("If you were asked to paste the Build Data from this module, please copy and paste the below information:");
        PutModule("------------------------------");
        PutModule("\x02AutoJoin:\x02 v3.1; \x02Born:\x02 Wed, June 26nd, 2019 @ 4:52pm EST;  \x02Release:\x02 Public");
    }

	void OnShowCommand(const CString& sLine) {
		
        PutModule("\x02 CURRENT SETTINGS\x02");
        PutModule("\x02 ---------------\x02");
        if (CString(m_bOVR) == "true") { 
        	PutModule("\x02 Deny All Invites:\x02\0034 ON\003");
			PutModule("\x02\0034 WARNING:\x02\003 While this setting is turned on, no invites will be accepted. To restore your normal settings, please set NOJOIN to off."); }
        if (CString(m_bOVR) == "false") {
        	PutModule("\x02 Deny All Invites:\x02\0033 OFF\003");
			if (CString(m_bAJC) == "true") {
				PutModule("\x02   Accept invites from C/ChanFix:\x02\0033 ON\x003");
			}
			else {
				PutModule("\x02   Accept invites from C/ChanFix:\x02\0034 OFF\x003");
			}
			PutModule("\x02     ChanFix Ident:\x02 " + CString(m_sCIdent));
			PutModule("\x02     ChanFix Host:\x02 " + CString(m_sCHost));

			if (CString(m_bAJX) == "true") {
				PutModule("\x02   Accept invites from X/CService:\x02\0033 ON\x003");
			}
			else {
				PutModule("\x02   Accept invites from X/CService:\x02\0034 OFF\x003");
			}
			PutModule("\x02     Channel Services Ident:\x02 " + CString(m_sXIdent));
			PutModule("\x02     Channel Services Host:\x02 " + CString(m_sXHost));

			if (CString(m_bAJEU) == "true") {
				PutModule("\x02   Accept invites from EUWorld:\x02\0033 ON\x003");
			}
			else {
				PutModule("\x02   Accept invites from EUWorld:\x02\0034 OFF\x003");
			}
			PutModule("\x02     EUWorld Ident:\x02 " + CString(m_sEUIdent));
			PutModule("\x02     EUWorld Host:\x02 " + CString(m_sEUHost));

			if (CString(m_bAJD) == "true") {
				PutModule("\x02   Accept invites from Security Services:\x02\0033 ON\x003");
			}
			else {
				PutModule("\x02   Accept invites from Security Services:\x02\0034 OFF\x003");
			}
			PutModule("\x02     Security Services Ident:\x02 " + CString(m_sDIdent));
			PutModule("\x02     Security Services Host:\x02 " + CString(m_sDHost));

			if (CString(m_sCustNick) == "\0035!!NOT SET!!\003") { m_bCBSU = false; } // Safety check. Ensure that the custom bot is defined.
			if (CString(m_sCustIdent) == "\0035!!NOT SET!!\003") { m_bCBSU = false; } // Safety check. Ensure that the custom bot is defined.
			if (CString(m_sCustHost) == "\0035!!NOT SET!!\003") { m_bCBSU = false; } // Safety check. Ensure that the custom bot is defined.
			if (CString(m_bCBSU) == "true") { 
				PutModule("\x02   Custom Bot Setup Completed:\x02\0033 YES\x003");
				if (CString(m_bCUST) == "true") {
					PutModule("\x02   Accept Invites from Custom Bot, " + CString(m_sCustNick) + ":\x02\0033 ON\x003");
				}
				else {
					PutModule("\x02   Accept Invites from Custom Bot, " + CString(m_sCustNick) + ":\x02\0034 OFF\x003");
				}
				PutModule("\x02     Custom Bot Nick:\x02 " + CString(m_sCustNick));
				PutModule("\x02     Custom Bot Ident:\x02 " + CString(m_sCustIdent));
				PutModule("\x02     Custom Bot Host:\x02 " + CString(m_sCustHost));
			}
			else { 
				PutModule("\x02   Custom Bot Setup Completed:\x02\0034 NO\x003"); 
				return;
			}
		}
	}

	void SetupCommand(const CString& sSubCmd) { 
		if (sSubCmd.Token(1).empty()) {
			PutModule("\x02 AVAILABLE OPTIONS\x02");
			PutModule("------------------------------");
			PutModule("\x02 C-Ident\x02 - Sets the ChanFix Ident");
			PutModule("\x02 C-Host\x02 - Sets the ChanFix Host");
			PutModule("\x02 D-Ident\x02 - Sets the Security Services Ident");
			PutModule("\x02 D-Host\x02 - Sets the Security Services Host");
			PutModule("\x02 EUWorld-Ident\x02 - Sets the EUWorld Ident");
			PutModule("\x02 EUWorld-Host\x02 - Sets the EUWorld Host");
			PutModule("\x02 X-Ident\x02 - Sets the Channel Services Ident");
			PutModule("\x02 X-Host\x02 - Sets the Channel Services Host");
			PutModule("------------------------------");
			PutModule("\0037\x02 Global-Host\003\x02 - Sets the host for all services. This will overwrite the host used for each service. Useful if your network isn't Undernet but uses a similar layout. Your custom defined bot is not affected by this setting.");
			PutModule("------------------------------");
			PutModule("\x02 Network-Name\x02 - Sets the Network Name. This is just a vanity option that modifies the output for the module and does not affect the operation of the module.");
			PutModule("------------------------------");
			PutModule("\0039\x02 Custom-Bot\003\x02 - Sets the nick, ident and host for a custom bot.");
		}
		if (sSubCmd.Token(1).Equals("C-Ident")) {
			if (sSubCmd.Token(2).empty()) {
				PutModule("\0034\x02 ERROR:\003\x02 You cannot have a blank ident. Please provide a valid ident.");
				return;
			}
			else { 
				m_sCIdent = sSubCmd.Token(2);
				SaveSettings();
				PutModule("\x02 Chanfix Ident changed to:\x02 " + CString(m_sCIdent));
			}
		}
		if (sSubCmd.Token(1).Equals("C-Host")) {
			if (sSubCmd.Token(2).empty()) {
				PutModule("\0034\x02 ERROR:\003\x02 You cannot have a blank host. Please provide a valid host.");
				return;
			}
			else {
				m_sCHost = sSubCmd.Token(2);
				SaveSettings();
				PutModule("\x02 Chanfix Host changed to:\x02 " + CString(m_sCHost));
			}
		}
		if (sSubCmd.Token(1).Equals("D-Ident")) {
			if (sSubCmd.Token(2).empty()) {
				PutModule("\0034\x02 ERROR:\003\x02 You cannot have a blank ident. Please provide a valid ident");
				return;
			}
			else {
				m_sDIdent = sSubCmd.Token(2);
				SaveSettings();
				PutModule("\x02 Security Services Ident changed to:\x02 " + CString(m_sDIdent));
			}
		}
		if (sSubCmd.Token(1).Equals("D-Host")) {
			if (sSubCmd.Token(2).empty()) {
				PutModule("\0034\x02 ERROR:\003\x02 You cannot have a blank host. Please provide a valid host.");
				return;
			}
			else {
				m_sDHost = sSubCmd.Token(2);
				SaveSettings();
				PutModule("\x02 Security Services Host changed to:\x02 " + CString(m_sDHost));
			}
		}
		if (sSubCmd.Token(1).Equals("EUWorld-Ident")) {
			if (sSubCmd.Token(2).empty()) {
				PutModule("\0034\x02 ERROR:\003\x02 You cannot have a blank ident. Please provide a valid ident.");
				return;
			}
			else {
				m_sEUIdent = sSubCmd.Token(2);
				SaveSettings();
				PutModule("\x02 EUWorld Ident changed to:\x02 " + CString(m_sEUIdent));
			}
		}
		if (sSubCmd.Token(1).Equals("EUWorld-Host")) {
			if (sSubCmd.Token(2).empty()) {
				PutModule("\0034\x02 ERROR:\003\x02 You cannot have a blank host. Please provide a valid host.");
				return;
			}
			else {
				m_sEUHost = sSubCmd.Token(2);
				SaveSettings();
				PutModule("\x02 EUWorld Host changed to:\x02 " + CString(m_sEUHost));
			}
		}
		if (sSubCmd.Token(1).Equals("X-Ident")) {
			if (sSubCmd.Token(2).empty()) {
				PutModule("\0034\x02 ERROR:\003\x02 You cannot have a blank ident. Please provide a valid ident.");
				return;
			}
			else { 
				m_sXIdent = sSubCmd.Token(2);
				SaveSettings();
				PutModule("\x02 Channel Services Ident changed to:\x02 " + CString(m_sXIdent));
			}
		}
		if (sSubCmd.Token(1).Equals("X-Host")) {
			if (sSubCmd.Token(2).empty()) {
				PutModule("\0034\x02 ERROR:\003\x02 You cannot have a blank host. Please provide a valid host.");
				return;
			}
			else {
				m_sXHost = sSubCmd.Token(2);
				SaveSettings();
				PutModule("\x02 Channel Services Host changed to:\x02 " + CString(m_sXHost));
			}
		}
		if (sSubCmd.Token(1).Equals("Global-Host")) {
			if (sSubCmd.Token(2).empty()) {
				PutModule("\0034\x02 ERROR:\003\x02 You cannot have a blank host. Please provide a valid host.");
				return;
			}
			else {
				m_sCHost = sSubCmd.Token(2);
				m_sDHost = sSubCmd.Token(2);
				m_sEUHost = sSubCmd.Token(2);
				m_sXHost = sSubCmd.Token(2);
				SaveSettings();
				PutModule("\x02 Host globally changed to:\x02 " + sSubCmd.Token(2));
				PutModule("\x02 Affected Services:\x02 ChanFix (C), Security Services (D), EUWorld, Channel Services (X)");
			}
		}
		if (sSubCmd.Token(1).Equals("Network-Name")) {
			if (sSubCmd.Token(2).empty()) {
				PutModule("\0034\x02 ERROR:\003\x02 You cannot have a blank network name. Please provide a valid network name.");
			}
			else { 
				m_sNetName = sSubCmd.Token(2);
				SaveSettings();
				PutModule("\x02 Network Name changed to:\x02 " + sSubCmd.Token(2));
			}
		}
		if (sSubCmd.Token(1).Equals("Custom-Bot")) {
			if (sSubCmd.Token(2).empty()) {
				PutModule("\0034\x02 ERROR:\003\x02 You must provide the nick of the bot this module will accept invites from.");
				PutModule("\x02 Syntax:\x02 Setup Custom-Bot \0034<nick> <ident> <host>\003");
			}
			else if (sSubCmd.Token(3).empty()) {
				PutModule("\0034\x02 ERROR:\003\x02 You must provide the ident of the bot this module will accept invites from.");
				PutModule("\x02 Syntax:\x02 Setup Custom-Bot \0033<nick>\003 \0034<ident> <host>\003");
			}
			else if (sSubCmd.Token(4).empty()) {
				PutModule("\0034\x02 ERROR:\003\x02 You must provide the host of the bot this module will accept invites from.");
				PutModule("\x02 Syntax:\x02 Setup Custom-Bot \0033<nick> <ident>\003 \0034<host>\003");
			}
			else {
				m_sCustNick = sSubCmd.Token(2);
				m_sCustIdent = sSubCmd.Token(3);
				m_sCustHost = sSubCmd.Token(4);
				m_bCBSU = true;
				SaveSettings();
				PutModule("\x02 Custom Bot Information saved:\x02 You will now accept invites from a bot that matches this hostmask - " + CString(m_sCustNick) + "!" + CString(m_sCustIdent) + "@" + CString(m_sCustHost));
				PutModule("\0037\x02 WARNING:\003\x02 Invites from custom bots can be risky. If the bot is compromised, it can be used to force you to join undesirable channels. To prevent this, you should only add a bot that has a CService Username. The bot should always have mode +x set.");
			}
		}
	}
};

template<> void TModInfo<CAcceptInvite>(CModInfo& Info) {
	Info.SetWikiPage("AcceptInvite");
}

NETWORKMODULEDEFS(CAcceptInvite, "Accepts secure invites received by network services and a single custom-defined bot on Undernet or an Undernet-based network. Can be configured.")

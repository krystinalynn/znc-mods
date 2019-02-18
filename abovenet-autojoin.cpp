//Channel Services Autojoin module by QueenElsa
//USAGE:
// -- Compile module.
// -- Load into ZNC (/znc loadmod autojoin) 
// -- Set a channel to AutoInvite (/msg Z modinfo #regchanname INVITE ON) You need 100 acc to higher to do this.
// -- Quit and Reconnect. 
//Enjoy! 

#include <znc/Chan.h>
#include <znc/IRCNetwork.h>
#include <znc/Modules.h>
#include <znc/Client.h>
using std::vector;
class Cautojoin : public CModule {
public:
        MODCONSTRUCTOR(Cautojoin) { PutStatus("\x02\00312 [MODULE: Autojoin] \x02\003 You will be AUTO-JOINED to channels when Z invites you."); }
        virtual ~Cautojoin() { PutStatus("\x02\0034  [MODULE: Autojoin] \x02\003 You will no longer be AUTO-JOINED to channels when Z invites you."); }
		virtual EModRet OnInvite(const CNick& Nick, const CString& sChan) override {
			if ((Nick.GetIdent() == "help")  && (Nick.GetHost() == "abovenet.org")) {
				PutModule("\x02\0033 [AUTO-JOINED (Channel Service)] \x02\003 Official AboveNet Service, " + Nick.GetNick() + " ("+ Nick.GetIdent() + "!" + Nick.GetHost() +") invited you to " + sChan + ", automatically joining.");
				PutIRC("JOIN " + sChan); 
				return CONTINUE;
			}
			else if ((Nick.GetIdent() =="aworld") && (Nick.GetHost() == "abovenet.org")) {
				PutModule("\x02\0036 [AUTO-JOINED (Operator Service)] \x02\003 Official AboveNet Service, " + Nick.GetNick() + " ("+ Nick.GetIdent() + "!" + Nick.GetHost() +") invited you to " + sChan + ", automatically joining."); 
				PutIRC("JOIN " + sChan);
				return CONTINUE; 
			}
			else if ((Nick.GetIdent() == "snowman") && (Nick.GetHost() == "olaf.users.abovenet.org")) { 
				PutModule("\x02\00313 [AUTO-JOINED (Trusted Bot)] \x02\003 Official Network Bot, " + Nick.GetNick() + " ("+ Nick.GetIdent() + "!" + Nick.GetHost() +") invited you to " + sChan + ", automatically joining."); 
				PutIRC("JOIN " + sChan); 
				return CONTINUE; 
			}
            /* Use the following template if you wish to add additional bots to the exempted list. 
            Color Code Syntax: \x02\003<mIRC Color Code>
            -- Example, mIRC color for Pink is 13 so line entry code to start pink would be \x02\00313
            -- Line termination code is ALWAYS \x02\003

			else if ((Nick.GetIdent() == "IDENT") && (Nick.GetHost() == "IP/HOST")) { 
				PutModule("\x02\00313 [AUTO-JOINED] \x02\003 IDENTIFIER, " + Nick.GetNick() + " ("+ Nick.GetIdent() + "!" + Nick.GetHost() +") invited you to " + sChan + ", automatically joining."); 
				PutIRC("JOIN " + sChan); 
				return CONTINUE; 
			}
            */
			else if ((Nick.GetIdent() == "snowman") && (Nick.GetHost() == "172.16.0.3")) { 
				PutModule("\x02\00313 [AUTO-JOINED (Trusted Bot)] \x02\003 QueenElsa's Bot, " + Nick.GetNick() + " ("+ Nick.GetIdent() + "!" + Nick.GetHost() +") invited you to " + sChan + ", automatically joining."); 
				PutIRC("JOIN " + sChan); 
				return CONTINUE; 
			}
			else { 
			PutModule("\x02\0034 [INVITE IGNORED (Untrusted User)] \x02\003 " + Nick.GetNick() + " ("+ Nick.GetIdent() + "!" + Nick.GetHost() +") invited you to " + sChan + " -- To accept, type: join " + sChan);
			return HALT;
			}
		}
		
		virtual void OnModCommand(const CString& sCommand) {
			if (sCommand.Token(0).Equals("JOIN")) {
				if (sCommand.Token(1).empty()) { PutModule("JOIN SYNTAX: join channel"); } 
					else {
					   PutModule("\x02\00312 [MANUALLY JOINED (Override)] \x02\003 " + sCommand.Token(1));
					   PutIRC("JOIN " + sCommand.Token(1));
					}
			}
			else if (sCommand.Equals("help")) {
				   PutModule("Available commands: JOIN");
				   PutModule("------------------------------");
				   PutModule("JOIN command");
				   PutModule("SYNTAX: join <#channel>");
				   PutModule("DESCRIPTION: If you received an ignored invite, typing JOIN <#channel> in this window will accept the invite, and join your client to the invited channel. To use this command with multiple channels at once, separate each channel with a comma.");
				   PutModule("EXAMPLE: JOIN #AboveNet,#cservice,#userguide");
				   PutModule("------------------------------");
			}
            else if (sCommand.Equals("elsa")) {
            PutStatus("[MODULE: Autojoin] QueenElsa is my original author. She wrote me from scratch and designed me for use on the AboveNet IRC network. If you're using me on another network, I might not function properly, since I'm hard-coded for use with AboveNet. If you have my source, feel free to adjust me as needed!");
			}
			else { 
			PutModule("I'm sorry, but I don't understand the command you entered: " + sCommand); 
			PutModule("If you need help with something, type HELP and I'll tell you what I can do.");
			}
		}	
};

NETWORKMODULEDEFS(Cautojoin, "Autojoin on /invite from AboveNet Channel Services.")

/*
template <>
void TModInfo<Cautojoin>(CModInfo& Info) {
    Info.SetWikiPage("Cautojoin");
    Info.SetHasArgs(false);
    Info.SetArgsHelpText("Autojoin on /invite from AboveNet Channel Services.");
}

MODULEDEFS(Cautojoin, "Autojoin on /invite from AboveNet Channel Services.")
*/



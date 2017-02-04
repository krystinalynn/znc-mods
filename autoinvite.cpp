//Channel Services Autojoin module by QueenElsa
//USAGE:
// -- Compile module.
// -- Load into ZNC (/znc loadmod cscautojoin) 
// -- Set a channel to AutoInvite (/msg X modinfo #regchanname INVITE ON) You need 100 acc to higher to do this.
// -- Quit and Reconnect. 
//Enjoy! 

#include <znc/Chan.h>
#include <znc/IRCNetwork.h>
#include <znc/Modules.h>
using std::vector;
class Cautojoin : public CModule {
public:
        MODCONSTRUCTOR(Cautojoin) { PutModule("You will be autojoined to channels when X invites you."); }
        virtual ~Cautojoin() { PutModule("You will no longer be autojoined to channels when X invites you."); }
		virtual EModRet OnInvite(const CNick& Nick, const CString& sChan) override {
			if ((Nick.GetIdent() == "cservice")  && (Nick.GetHost() == "undernet.org")) {
				PutModule("[AUTOJOINED] Network Service, " + Nick.GetNick() + " invited us to " + sChan + " automatically joining.");
				PutIRC("JOIN " + sChan); 
				return CONTINUE;
			}
			PutModule("[IGNORED] " + Nick.GetNick() + " invited us to " + sChan + " -- To accept, please /join " + sChan + " manually.");
			return HALT;
		}
};
NETWORKMODULEDEFS(Cautojoin, "Autojoin on /invite from Undernet Channel Services.")
/*
template <>
void TModInfo<Cautojoin>(CModInfo& Info) {
    Info.SetWikiPage("Cautojoin");
    Info.SetHasArgs(false);
    Info.SetArgsHelpText("Autojoin on /invite from Undernet Channel Services.");
}

MODULEDEFS(Cautojoin, "Autojoin on /invite from Undernet Channel Services.")
*/

#include "Changes.h"
#include "P4Command.h"
#include "P4Task.h"

using namespace std;

class P4RevertChangesCommand : public P4Command
{
public:
	P4RevertChangesCommand(const char* name) : P4Command(name){}
	virtual bool Run(P4Task& task, const CommandArgs& args)
	{
		ClearStatus();
		upipe.Log() << args[0] << "::Run()" << endl;
		std::string errorMessage;
		
		const string cmd = args.size() > 1 && args[1] == "unchangedOnly" ? 
		"revert -a -c " :
		"revert -c ";
		
		ChangelistRevisions changes;
		upipe >> changes;
		
		if (changes.empty())
		{
			upipe.WarnLine("Changes to revert is empty", MARemote);
			upipe.EndResponse();
			return true;
		}
		
		string cl;
		for (ChangelistRevisions::const_iterator i = changes.begin(); i != changes.end(); ++i)
		{
			string rev = *i == kDefaultListRevision ? string("default") : *i;
			string cmdr = cmd + " \"" + rev + "\"";

			if (!task.CommandRun(cmdr, this))
				break;
		}
		
		// The OutputState and other callbacks will now output to stdout.
		// We just wrap up the communication here.
		upipe << GetStatus();

		// @TODO: send changed assets
		VersionedAssetList dummy;
		upipe << dummy;
		
		upipe.EndResponse();
		
		return true;
	}
} cRevertChanges("revertChanges");

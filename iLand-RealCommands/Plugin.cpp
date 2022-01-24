#include "pch.h"
#include <EventAPI.h>
#include <LoggerAPI.h>

using namespace LL;
using namespace std;
Logger logger("ILand");

bool lockRegistry = true;

const vector<string> cmdmap = {
	"land",
	"land new",
	"land giveup",
	"land gui",
	"land set",
	"land buy",
	"land ok",
	"land mgr",
	"land mgr selectool",
	"land tp",
	"land tp set",
	"land tp rm",
	"land op",
	"land deop",
	"land update",
	"land language",
	"land language set",
	"land language list",
	"land language list-online",
	"land language install",
	"land language update",
	"land reload",
	"land unload"
};

class LandCommand : public Command {
	enum class OperationType {
		create,	//new
		giveup,
		gui,
		set,
		buy,
		ok,
		mgr,
		tp,
		op,
		deop,
		update,
		language,
		reload,
		unload
	} Operation;
public:
	void execute(CommandOrigin const& ori, CommandOutput& output) const override {
	};
	static void setup(CommandRegistry* registry) {
		lockRegistry = false;
		registry->registerCommand(
			"land",
			"land command.",
			CommandPermissionLevel::HostPlayer,
			{ (CommandFlagValue)0 },
			{ (CommandFlagValue)0x80 }
		);
		registry->addEnum<OperationType>("OperationType",
			{
				{"buy",OperationType::buy},
				{"new",OperationType::create},
				{"deop",OperationType::deop},
				{"giveup",OperationType::giveup},
				{"gui",OperationType::gui},
				{"language",OperationType::language},
				{"mgr",OperationType::mgr},
				{"ok",OperationType::ok},
				{"op",OperationType::op},
				{"reload",OperationType::reload},
				{"set",OperationType::set},
				{"tp",OperationType::tp},
				{"unload",OperationType::unload},
				{"update",OperationType::update}
			}
		);
		registry->registerOverload<LandCommand>(
			"land",
			""
		)
		lockRegistry = true;
	};
};

void PluginMain()
{
	logger.info("loaded.");
}

void PluginInit()
{
	registerPlugin("", "", Version{ 1,0,0,Version::Beta }, { {"Author","Redbeanw"} });
	Event::ServerStartedEvent::subscribe([](Event::ServerStartedEvent ev) -> bool {
		PluginMain();
		return true;
	});
	Event::RegCmdEvent::subscribe([](Event::RegCmdEvent ev) -> bool {
		LandCommand::setup(ev.mCommandRegistry);
		return true;
	});
}

THook(void, "?registerCommand@CommandRegistry@@QEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@PEBDW4CommandPermissionLevel@@UCommandFlag@@3@Z",
	void* self, string* cmdstr, void* a3, char* a4, short* a5, short* a6)
{
	if (lockRegistry && count(cmdmap.begin(), cmdmap.end(), *cmdstr))
		return;
	original(self, cmdstr, a3, a4, a5, a6);
}

/*
#include <MC/CommandContext.hpp>
THook(MCRESULT*, "?executeCommand@MinecraftCommands@@QEBA?AUMCRESULT@@V?$shared_ptr@VCommandContext@@@std@@_N@Z",
	MinecraftCommands* self , MCRESULT* rtn, std::shared_ptr<CommandContext> context, bool print)
*/
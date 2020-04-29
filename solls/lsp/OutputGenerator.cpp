#include "OutputGenerator.h"
#include <libsolutil/Visitor.h>
#include <variant>

namespace lsp {

using namespace std;

OutputGenerator::NotificationInfo OutputGenerator::operator()(protocol::Notification const& _message)
{
	return visit(*this, _message);
}

OutputGenerator::NotificationInfo OutputGenerator::operator()(protocol::CancelRequest const& _message)
{
	Json::Value reply;

	visit(
		solidity::util::GenericVisitor{
			[&](int _id) {
				reply["id"] = _id;
			},
			[&](string const& _id) {
				reply["id"] = _id;
			}
		},
		_message.id
	);

	return {"$/cancelRequest", reply};
}

OutputGenerator::NotificationInfo OutputGenerator::operator()(protocol::PublishDiagnosticsParams const&)
{
	Json::Value reply;

	// TODO

	return {"textDocument/publishDiagnostics", reply};
}

Json::Value OutputGenerator::operator()(protocol::Response const& _response)
{
	return visit(*this, _response);
}

Json::Value OutputGenerator::operator()(protocol::InitializeResult const& _response)
{
	Json::Value reply;

	if (_response.serverInfo.has_value())
	{
		reply["serverInfo"]["name"] = _response.serverInfo->name;
		if (_response.serverInfo->version.has_value())
			reply["serverInfo"]["version"] = _response.serverInfo->version.value();
	}

	if (_response.capabilities.hoverProvider)
		reply["hoverProvider"] = true;

	reply["capabilities"]["hoverProvider"] = _response.capabilities.hoverProvider;
	reply["capabilities"]["textDocumentSync"]["openClose"] = _response.capabilities.textDocumentSync.openClose;
	reply["capabilities"]["textDocumentSync"]["change"] = static_cast<int>(_response.capabilities.textDocumentSync.change);

	return reply;
}

} // end namespace
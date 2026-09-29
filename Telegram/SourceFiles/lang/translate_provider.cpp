/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lang/translate_provider.h"

#include "base/options.h"
#include "core/application.h"
#include "core/core_settings.h"
#include "data/data_msg_id.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "history/history_item.h"
#include "lang/translate_mtproto_provider.h"
#include "lang/translate_url_provider.h"
#include "platform/platform_translate_provider.h"

#include <QtCore/QLocale>

namespace {

base::options::option<QString> OptionTranslateUrlTemplate({
	.id = "translate-url-template",
	.name = "Translate URL template",
	.description = "Template URL for custom translation provider."
		" Supports %q text, %f source language and %t target language.",
});

base::options::option<QString> OptionTranslateOutLanguage({
	.id = "translate-out-language",
	.name = "Translate outgoing to",
	.description = "Two letter language code the composer translate action"
		" turns the draft into, e.g. \"en\".",
	.defaultValue = "en",
});

// hw: vendor-parity options. Values are strings, as base::options::Read
// requires top-level string values in tdata/experimental_options.json.
base::options::option<QString> OptionTranslateAutoIn({
	.id = "translate-auto-in",
	.name = "Translate incoming automatically",
	.description = "Translate every incoming message automatically, including"
		" private chats and small groups, e.g. \"true\".",
	.defaultValue = "false",
});

base::options::option<QString> OptionTranslateInLanguage({
	.id = "translate-in-language",
	.name = "Translate incoming to",
	.description = "Two letter language code incoming messages are translated"
		" into, e.g. \"zh\".",
	.defaultValue = "zh",
});

base::options::option<QString> OptionTranslateAutoOut({
	.id = "translate-auto-out",
	.name = "Translate outgoing automatically",
	.description = "Translate the draft automatically while typing and replace"
		" it with the translation, e.g. \"true\".",
	.defaultValue = "false",
});

base::options::option<QString> OptionTranslateBilingual({
	.id = "translate-bilingual",
	.name = "Show original and translation",
	.description = "Show the translation below the original text instead of"
		" replacing the original, e.g. \"true\".",
	.defaultValue = "true",
});

} // namespace

namespace Ui {

std::unique_ptr<TranslateProvider> CreateTranslateProvider(
		not_null<Main::Session*> session) {
	const auto urlTemplate = OptionTranslateUrlTemplate.value();
	if (!urlTemplate.isEmpty()
		&& urlTemplate.contains(u"%q"_q)) {
		return CreateUrlTranslateProvider(urlTemplate);
	}
	if (Core::App().settings().usePlatformTranslation()
		&& Platform::IsTranslateProviderAvailable()) {
		return Platform::CreateTranslateProvider();
	}
	return CreateMTProtoTranslateProvider(session);
}

QString TranslateOutLanguageCode() {
	const auto code = OptionTranslateOutLanguage.value().trimmed();
	return code.isEmpty() ? u"en"_q : code;
}

LanguageId TranslateOutLanguage() {
	const auto locale = QLocale(TranslateOutLanguageCode());
	const auto language = locale.language();
	return LanguageId{
		(language != QLocale::C) ? language : QLocale::English };
}

namespace {

// hw: string options are used as booleans on purpose.
[[nodiscard]] bool HwOptionEnabled(const QString &value) {
	const auto normalized = value.trimmed().toLower();
	return (normalized == u"true"_q)
		|| (normalized == u"1"_q)
		|| (normalized == u"yes"_q)
		|| (normalized == u"on"_q);
}

} // namespace

bool TranslateAutoIn() {
	return HwOptionEnabled(OptionTranslateAutoIn.value());
}

LanguageId TranslateInLanguage() {
	const auto code = OptionTranslateInLanguage.value().trimmed();
	const auto locale = QLocale(code.isEmpty() ? u"zh"_q : code);
	const auto language = locale.language();
	return LanguageId{
		(language != QLocale::C) ? language : QLocale::Chinese };
}

bool TranslateAutoOut() {
	return HwOptionEnabled(OptionTranslateAutoOut.value());
}

bool TranslateBilingual() {
	return HwOptionEnabled(OptionTranslateBilingual.value());
}

TranslateProviderRequest PrepareTranslateProviderRequest(
		not_null<TranslateProvider*> provider,
		not_null<PeerData*> peer,
		MsgId msgId,
		TextWithEntities text) {
	auto result = TranslateProviderRequest{
		.peerId = uint64(peer->id.value),
		.msgId = IsServerMsgId(msgId) ? msgId.bare : 0,
		.text = std::move(text),
	};
	if (provider->supportsMessageId()) {
		return result;
	}
	if (result.msgId) {
		if (const auto i = peer->owner().message(peer, MsgId(result.msgId))) {
			result.text = i->originalText();
		}
		result.msgId = 0;
	}
	return result;
}

} // namespace Ui

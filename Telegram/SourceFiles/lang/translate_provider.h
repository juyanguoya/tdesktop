/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include <translate_provider.h>

class PeerData;
struct MsgId;

namespace Main {
class Session;
} // namespace Main

namespace Ui {

[[nodiscard]] std::unique_ptr<TranslateProvider> CreateTranslateProvider(
	not_null<Main::Session*> session);

[[nodiscard]] QString TranslateOutLanguageCode();

[[nodiscard]] LanguageId TranslateOutLanguage();

// hw: auto-translation of incoming messages (vendor parity).
[[nodiscard]] bool TranslateAutoIn();

[[nodiscard]] LanguageId TranslateInLanguage();

// hw: auto-translation of the draft while typing (vendor parity).
[[nodiscard]] bool TranslateAutoOut();

// hw: show the original text above the translation.
[[nodiscard]] bool TranslateBilingual();

[[nodiscard]] TranslateProviderRequest PrepareTranslateProviderRequest(
	not_null<TranslateProvider*> provider,
	not_null<PeerData*> peer,
	MsgId msgId,
	TextWithEntities text);

} // namespace Ui

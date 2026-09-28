# own-translate

Fork of [telegramdesktop/tdesktop](https://github.com/telegramdesktop/tdesktop)
(GPL v3) that keeps only what we need and points translation at our own server.

## Changes vs upstream (branch `own-translate`)

| file | change |
|---|---|
| `main/main_domain.h` | `kMaxAccounts` 3 -> 100, `kPremiumMaxAccounts` 6 -> 100 (multi-account beyond the stock 3/6 limit) |
| `history/view/history_view_translate_tracker.cpp` | drop the Telegram-Premium requirement for whole-chat auto-translation |
| `boxes/language_box.cpp` | un-gate the "translate whole chat" toggle (locked style / locked toggle / buy-premium popup) |
| `lang/translate_provider.cpp/.h` | new experimental option `translate-out-language` (default `"en"`) + `TranslateOutLanguageCode()` / `TranslateOutLanguage()` |
| `history/view/controls/history_view_compose_controls.cpp` | **outgoing translation**: right-click in the message box -> "Translate draft (EN)" replaces the draft with its translation before sending |
| `Telegram/build/prepare/prepare.py` | install `diffutils` (MSYS2) instead of the removed `mingw-w64-x86_64-diffutils` |

The incoming-translation path is **not** patched in code: upstream already supports
a custom translation endpoint via the experimental option `translate-url-template`
(see `lang/translate_provider.cpp`; `core/launcher.cpp` reads
`<workingdir>/tdata/experimental_options.json`).

## Backend contract (`GET <template>`)

`%q` = source text (already HTML-escaped and percent-encoded by the client),
`%f` = source language or `auto`, `%t` = two-letter target language.

The response must be a Google-style segmented array, because that is what
`translate_url_provider.cpp` parses:

```json
[[["translated text"]]]
```

Non-200 responses are shown as a translation failure. Note the client escapes the
source text with `EscapeForHtml()` before percent-encoding, so a backend should
`html.unescape()` what it receives.

## Deploy

1. put `experimental_options.json` (see the `.example`) into `<client dir>/tdata/`
2. the backend must answer `GET <template>` as described above

**The API key is deliberately NOT in this repository.** The template example only
carries placeholders; the real `experimental_options.json` is generated per
installation by the deployment script.

## License

GPL v3 with OpenSSL exception, as upstream. Source is offered here to anyone who
receives a binary built from this branch.

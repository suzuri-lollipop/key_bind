#include <iostream>
#include <windows.h>
#include <string>
#include <vector>

// 座標とクリックに必要なキーコードを保持する構造体
struct ClickSetting {
    int x, y;
    std::vector<int> keyCodes;
};

std::vector<ClickSetting> ClickPoints;  // クリックの設定を保持するリスト
bool RecordCoordinates = false;          // 座標を記録するかどうかのフラグ

// キーコンボ文字列を解析して、対応するキーコードを取得する
std::vector<int> ParseKeyCombo(const std::wstring& combo) {
    std::vector<int> keyCodes;

    if (combo.find(L"ALT") != std::wstring::npos) {
        keyCodes.push_back(VK_MENU);
    }

    if (combo.find(L"CTRL") != std::wstring::npos) {
        keyCodes.push_back(VK_CONTROL);
    }

    if (combo.find(L"SHIFT") != std::wstring::npos) {
        keyCodes.push_back(VK_SHIFT);
    }

    // 単一文字のキーを探す
    size_t lastComma = combo.find_last_of(L',');
    if (lastComma != std::wstring::npos && lastComma + 2 <= combo.size()) {
        wchar_t ch = combo[lastComma + 1];
        if (iswupper(ch)) {
            keyCodes.push_back(ch);
        }
    }

    return keyCodes;
}

// INIファイルから設定をロードする
void LoadSettingsFromINI(const std::wstring& iniPath) {
    RecordCoordinates = GetPrivateProfileIntW(L"Settings", L"RecordCoordinates", 0, iniPath.c_str());

    for (int i = 0; ; i++) {
        std::wstring keyX = L"Point" + std::to_wstring(i) + L"_X";
        std::wstring keyY = L"Point" + std::to_wstring(i) + L"_Y";
        std::wstring keyCombo = L"Point" + std::to_wstring(i) + L"_KeyCombo";

        wchar_t comboBuffer[100];
        GetPrivateProfileStringW(L"Coordinates", keyCombo.c_str(), L"", comboBuffer, 100, iniPath.c_str());

        int x = GetPrivateProfileIntW(L"Coordinates", keyX.c_str(), 0, iniPath.c_str());
        int y = GetPrivateProfileIntW(L"Coordinates", keyY.c_str(), 0, iniPath.c_str());

        std::vector<int> keyCodes = ParseKeyCombo(comboBuffer);

        if (x == 0 && y == 0 && keyCodes.empty()) {
            break;  // 設定が見つからない場合はループを終了
        }

        ClickPoints.push_back({ x, y, keyCodes });
    }
}

// 指定した座標でクリックを実行する
void ClickAtPoint(int x, int y) {
    SetCursorPos(x, y);
    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

int main() {
    LoadSettingsFromINI(L"./settings.ini");

    while (true) {
        POINT originalPoint;

        if (RecordCoordinates) {
            GetCursorPos(&originalPoint);
        }

        for (const auto& setting : ClickPoints) {
            bool allKeysPressed = true;
            for (int code : setting.keyCodes) {
                if (!(GetAsyncKeyState(code) & 0x8000)) {
                    allKeysPressed = false;
                    break;
                }
            }

            if (allKeysPressed) {
                RECT rc;
                rc.left = setting.x;
                rc.top = setting.y;
                rc.right = setting.x;
                rc.bottom = setting.y;
                ClipCursor(&rc);

                ClickAtPoint(setting.x, setting.y);

                ClipCursor(NULL);

                if (RecordCoordinates) {
                    SetCursorPos(originalPoint.x, originalPoint.y);
                }
            }
        }

        Sleep(1);
    }
    return 0;
}
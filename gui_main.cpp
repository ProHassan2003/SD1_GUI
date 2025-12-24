
#include <windows.h>
#include <commdlg.h>
#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <cstring>

#include "resource.h"
#include "FileGenerator.h"
#include "Processor.h"
#include "Timer.h"

static HINSTANCE g_hInst = nullptr;
static HWND g_hMainWnd = nullptr;
static HWND g_hEditLog = nullptr;

static std::filesystem::path g_inputFile;

static void AppendLog(const std::wstring& text) {
    if (!g_hEditLog) return;
    int len = GetWindowTextLengthW(g_hEditLog);
    SendMessageW(g_hEditLog, EM_SETSEL, (WPARAM)len, (LPARAM)len);
    SendMessageW(g_hEditLog, EM_REPLACESEL, FALSE, (LPARAM)text.c_str());
    SendMessageW(g_hEditLog, EM_SCROLLCARET, 0, 0);
}

static void AppendLine(const std::wstring& line) {
    AppendLog(line + L"\r\n");
}

static std::wstring GetLastErrorMessage() {
    DWORD err = GetLastError();
    if (!err) return L"";
    LPWSTR buf = nullptr;
    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&buf, 0, nullptr);
    std::wstring msg = buf ? buf : L"";
    if (buf) LocalFree(buf);
    return msg;
}

static bool ShowOpenFileDialog(HWND owner, std::filesystem::path& outPath) {
    wchar_t fileBuf[MAX_PATH] = { 0 };

    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = owner;
    ofn.lpstrFilter = L"Text files (*.txt)\0*.txt\0All files (*.*)\0*.*\0";
    ofn.lpstrFile = fileBuf;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_EXPLORER;
    ofn.lpstrTitle = L"Open student data file";

    if (GetOpenFileNameW(&ofn)) {
        outPath = fileBuf;
        return true;
    }
    return false;
}

static bool ShowSaveFileDialog(HWND owner, std::filesystem::path& outPath, const wchar_t* title, const wchar_t* defaultName) {
    wchar_t fileBuf[MAX_PATH] = { 0 };
    if (defaultName) {
        wcsncpy_s(fileBuf, defaultName, _TRUNCATE);
    }

    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = owner;
    ofn.lpstrFilter = L"Text files (*.txt)\0*.txt\0All files (*.*)\0*.*\0";
    ofn.lpstrFile = fileBuf;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_EXPLORER;
    ofn.lpstrTitle = title;

    if (GetSaveFileNameW(&ofn)) {
        outPath = fileBuf;
        return true;
    }
    return false;
}

struct GenerateParams {
    long long count = 1000;
};

static INT_PTR CALLBACK GenerateDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
    GenerateParams* p = reinterpret_cast<GenerateParams*>(GetWindowLongPtrW(hDlg, GWLP_USERDATA));

    switch (msg) {
    case WM_INITDIALOG: {
        SetWindowLongPtrW(hDlg, GWLP_USERDATA, lParam);
        p = reinterpret_cast<GenerateParams*>(lParam);
        SetDlgItemInt(hDlg, IDC_EDIT_COUNT, (UINT)p->count, FALSE);
        return TRUE;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK: {
            BOOL ok = FALSE;
            UINT v = GetDlgItemInt(hDlg, IDC_EDIT_COUNT, &ok, FALSE);
            if (!ok || v == 0) {
                MessageBoxW(hDlg, L"Please enter a positive number.", L"Invalid input", MB_ICONWARNING);
                return TRUE;
            }
            p->count = (long long)v;
            EndDialog(hDlg, IDOK);
            return TRUE;
        }
        case IDCANCEL:
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

struct ProcessParams {
    int container = 0; // 0 vector, 1 list, 2 deque
    int strategy = 0;  // 0 strategy1, 1 strategy2
};

static INT_PTR CALLBACK ProcessDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
    ProcessParams* p = reinterpret_cast<ProcessParams*>(GetWindowLongPtrW(hDlg, GWLP_USERDATA));
    switch (msg) {
    case WM_INITDIALOG: {
        SetWindowLongPtrW(hDlg, GWLP_USERDATA, lParam);
        p = reinterpret_cast<ProcessParams*>(lParam);

        HWND comboContainer = GetDlgItem(hDlg, IDC_COMBO_CONTAINER);
        SendMessageW(comboContainer, CB_ADDSTRING, 0, (LPARAM)L"std::vector");
        SendMessageW(comboContainer, CB_ADDSTRING, 0, (LPARAM)L"std::list");
        SendMessageW(comboContainer, CB_ADDSTRING, 0, (LPARAM)L"std::deque");
        SendMessageW(comboContainer, CB_SETCURSEL, p->container, 0);

        HWND comboStrategy = GetDlgItem(hDlg, IDC_COMBO_STRATEGY);
        SendMessageW(comboStrategy, CB_ADDSTRING, 0, (LPARAM)L"Strategy 1: copy passed + failed");
        SendMessageW(comboStrategy, CB_ADDSTRING, 0, (LPARAM)L"Strategy 2: move failed, keep only passed");
        SendMessageW(comboStrategy, CB_SETCURSEL, p->strategy, 0);
        return TRUE;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK: {
            HWND comboContainer = GetDlgItem(hDlg, IDC_COMBO_CONTAINER);
            HWND comboStrategy = GetDlgItem(hDlg, IDC_COMBO_STRATEGY);
            p->container = (int)SendMessageW(comboContainer, CB_GETCURSEL, 0, 0);
            p->strategy = (int)SendMessageW(comboStrategy, CB_GETCURSEL, 0, 0);
            EndDialog(hDlg, IDOK);
            return TRUE;
        }
        case IDCANCEL:
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

static void DoGenerate(HWND owner) {
    std::filesystem::path outFile;
    if (!ShowSaveFileDialog(owner, outFile, L"Save generated data file", L"students_1000.txt")) return;

    GenerateParams params;
    if (DialogBoxParamW(g_hInst, MAKEINTRESOURCEW(IDD_GENERATE), owner, GenerateDlgProc, (LPARAM)&params) != IDOK) {
        return;
    }

    try {
        AppendLine(L"Generating file...");
        AppendLine(L"Output: " + outFile.wstring());
        AppendLine(L"Records: " + std::to_wstring(params.count));

        Timer t;
        t.start();
        FileGenerator::generateStudentFile(outFile, params.count);
        t.stop();

        AppendLine(L"Done. Generation time: " + std::to_wstring(t.getMilliseconds()) + L" ms");
        AppendLine(L"");
    }
    catch (const std::exception& ex) {
        std::wstring msg = L"Generation failed.\r\n\r\n";
        msg += std::wstring(ex.what(), ex.what() + strlen(ex.what()));
        MessageBoxW(owner, msg.c_str(), L"Error", MB_ICONERROR);
    }
}

static std::filesystem::path WithSuffix(const std::filesystem::path& base, const std::wstring& suffix) {
    auto dir = base.parent_path();
    auto stem = base.stem().wstring();
    auto ext = base.extension().wstring();
    if (ext.empty()) ext = L".txt";
    return dir / (stem + suffix + ext);
}

static void DoProcess(HWND owner) {
    if (g_inputFile.empty()) {
        std::filesystem::path in;
        if (!ShowOpenFileDialog(owner, in)) return;
        g_inputFile = in;
        AppendLine(L"Selected input file: " + g_inputFile.wstring());
        AppendLine(L"");
    }

    ProcessParams p;
    if (DialogBoxParamW(g_hInst, MAKEINTRESOURCEW(IDD_PROCESS), owner, ProcessDlgProc, (LPARAM)&p) != IDOK) {
        return;
    }

    std::filesystem::path passedFile;
    if (!ShowSaveFileDialog(owner, passedFile, L"Save PASSED file as", L"passed.txt")) return;
    std::filesystem::path failedFile = WithSuffix(passedFile, L"_failed");

    try {
        AppendLine(L"Processing...");
        AppendLine(L"Input: " + g_inputFile.wstring());
        AppendLine(L"Passed output: " + passedFile.wstring());
        AppendLine(L"Failed output: " + failedFile.wstring());

        Timer timer;
        double readMs = 0, sortMs = 0, splitMs = 0, writeMs = 0;

        if (p.container == 0) {
            AppendLine(L"Container: std::vector");
            timer.start();
            auto students = Processor::readStudentsVector(g_inputFile);
            timer.stop(); readMs = timer.getMilliseconds();

            timer.start();
            Processor::sortStudents(students);
            timer.stop(); sortMs = timer.getMilliseconds();

            std::vector<Student> passed, failed;
            if (p.strategy == 0) {
                AppendLine(L"Strategy: 1 (copy)");
                timer.start();
                Processor::splitStrategy1(students, passed, failed);
                timer.stop(); splitMs = timer.getMilliseconds();
            }
            else {
                AppendLine(L"Strategy: 2 (move failed, keep passed)");
                timer.start();
                Processor::splitStrategy2(students, failed);
                passed = std::move(students);
                timer.stop(); splitMs = timer.getMilliseconds();
            }

            timer.start();
            Processor::saveStudents(passedFile, passed);
            Processor::saveStudents(failedFile, failed);
            timer.stop(); writeMs = timer.getMilliseconds();
        }
        else if (p.container == 1) {
            AppendLine(L"Container: std::list");
            timer.start();
            auto students = Processor::readStudentsList(g_inputFile);
            timer.stop(); readMs = timer.getMilliseconds();

            timer.start();
            Processor::sortStudents(students);
            timer.stop(); sortMs = timer.getMilliseconds();

            std::list<Student> failed;
            if (p.strategy == 0) {
                AppendLine(L"Strategy: 1 (copy)");
                std::list<Student> passed;
                timer.start();
                Processor::splitStrategy1(students, passed, failed);
                timer.stop(); splitMs = timer.getMilliseconds();

                timer.start();
                Processor::saveStudents(passedFile, passed);
                Processor::saveStudents(failedFile, failed);
                timer.stop(); writeMs = timer.getMilliseconds();
            }
            else {
                AppendLine(L"Strategy: 2 (move failed, keep passed)");
                timer.start();
                Processor::splitStrategy2(students, failed);
                timer.stop(); splitMs = timer.getMilliseconds();

                timer.start();
                Processor::saveStudents(passedFile, students);
                Processor::saveStudents(failedFile, failed);
                timer.stop(); writeMs = timer.getMilliseconds();
            }
        }
        else {
            AppendLine(L"Container: std::deque");
            timer.start();
            auto students = Processor::readStudentsDeque(g_inputFile);
            timer.stop(); readMs = timer.getMilliseconds();

            timer.start();
            Processor::sortStudents(students);
            timer.stop(); sortMs = timer.getMilliseconds();

            std::deque<Student> failed;
            if (p.strategy == 0) {
                AppendLine(L"Strategy: 1 (copy)");
                std::deque<Student> passed;
                timer.start();
                Processor::splitStrategy1(students, passed, failed);
                timer.stop(); splitMs = timer.getMilliseconds();

                timer.start();
                Processor::saveStudents(passedFile, passed);
                Processor::saveStudents(failedFile, failed);
                timer.stop(); writeMs = timer.getMilliseconds();
            }
            else {
                AppendLine(L"Strategy: 2 (move failed, keep passed)");
                timer.start();
                Processor::splitStrategy2(students, failed);
                timer.stop(); splitMs = timer.getMilliseconds();

                timer.start();
                Processor::saveStudents(passedFile, students);
                Processor::saveStudents(failedFile, failed);
                timer.stop(); writeMs = timer.getMilliseconds();
            }
        }

        AppendLine(L"");
        AppendLine(L"Performance summary (ms):");
        AppendLine(L"Read:  " + std::to_wstring(readMs));
        AppendLine(L"Sort:  " + std::to_wstring(sortMs));
        AppendLine(L"Split: " + std::to_wstring(splitMs));
        AppendLine(L"Write: " + std::to_wstring(writeMs));
        AppendLine(L"");
        AppendLine(L"Done.");
        AppendLine(L"");
    }
    catch (const std::exception& ex) {
        std::wstring msg = L"Processing failed.\r\n\r\n";
        msg += std::wstring(ex.what(), ex.what() + strlen(ex.what()));
        MessageBoxW(owner, msg.c_str(), L"Error", MB_ICONERROR);
    }
}

static void DoOpen(HWND owner) {
    std::filesystem::path in;
    if (!ShowOpenFileDialog(owner, in)) return;
    g_inputFile = in;
    AppendLine(L"Selected input file: " + g_inputFile.wstring());
    AppendLine(L"");
}

static void DoSaveLog(HWND owner) {
    std::filesystem::path out;
    if (!ShowSaveFileDialog(owner, out, L"Save log to file", L"log.txt")) return;

    int len = GetWindowTextLengthW(g_hEditLog);
    std::wstring buf(len, L'\0');
    GetWindowTextW(g_hEditLog, buf.data(), len + 1);

    try {
        std::wofstream f(out);
        f.write(buf.c_str(), (std::streamsize)buf.size());
        AppendLine(L"Log saved: " + out.wstring());
        AppendLine(L"");
    }
    catch (...) {
        MessageBoxW(owner, L"Could not save the log file.", L"Error", MB_ICONERROR);
    }
}

static void DoAbout(HWND owner) {
    const wchar_t* text =
        L"Student Processing System (SD1 v1.0 GUI)\r\n\r\n"
        L"This application wraps the SD1 console logic into a Windows GUI.\r\n"
        L"Use File menu to open data files or save the log.\r\n"
        L"Use Tools menu to generate data and process the selected file.\r\n";
    MessageBoxW(owner, text, L"About", MB_ICONINFORMATION);
}

static void CreateLayout(HWND hWnd) {
    RECT rc{};
    GetClientRect(hWnd, &rc);

    g_hEditLog = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        L"",
        WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY | WS_VSCROLL,
        8, 8, (rc.right - rc.left) - 16, (rc.bottom - rc.top) - 16,
        hWnd,
        (HMENU)IDC_EDIT_LOG,
        g_hInst,
        nullptr
    );

    HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    SendMessageW(g_hEditLog, WM_SETFONT, (WPARAM)hFont, TRUE);
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        CreateLayout(hWnd);
        AppendLine(L"Ready. Use Tools -> Generate or Process.");
        return 0;

    case WM_SIZE:
        if (g_hEditLog) {
            RECT rc{};
            GetClientRect(hWnd, &rc);
            MoveWindow(g_hEditLog, 8, 8, (rc.right - rc.left) - 16, (rc.bottom - rc.top) - 16, TRUE);
        }
        return 0;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDM_FILE_OPEN:
            DoOpen(hWnd);
            return 0;
        case IDM_FILE_SAVE_LOG:
            DoSaveLog(hWnd);
            return 0;
        case IDM_FILE_EXIT:
            DestroyWindow(hWnd);
            return 0;

        case IDM_TOOLS_GENERATE:
            DoGenerate(hWnd);
            return 0;
        case IDM_TOOLS_PROCESS:
            DoProcess(hWnd);
            return 0;

        case IDM_HELP_ABOUT:
            DoAbout(hWnd);
            return 0;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow) {
    g_hInst = hInstance;

    const wchar_t CLASS_NAME[] = L"StudentGradesGuiWindow";

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = CLASS_NAME;

    if (!RegisterClassExW(&wc)) {
        MessageBoxW(nullptr, (L"RegisterClassEx failed: " + GetLastErrorMessage()).c_str(), L"Error", MB_ICONERROR);
        return 0;
    }

    HWND hWnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"Student Processing System (GUI)",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 980, 640,
        nullptr,
        LoadMenuW(hInstance, MAKEINTRESOURCEW(IDR_MAINMENU)),
        hInstance,
        nullptr
    );

    if (!hWnd) {
        MessageBoxW(nullptr, (L"CreateWindowEx failed: " + GetLastErrorMessage()).c_str(), L"Error", MB_ICONERROR);
        return 0;
    }

    g_hMainWnd = hWnd;
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return (int)msg.wParam;
}

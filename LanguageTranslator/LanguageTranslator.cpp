// LanguageTranslator.cpp : Defines the entry point for the application.


//Because of wcscpy you have to do these steps:
//Mark all the desired projects in solution explorer.
//Press Alt - F7 or right click in solution explorer and select "Properties"
//Configurations:All Configurations
//Click on the Preprocessor Definitions line to invoke its editor
//Choose Edit...
//Copy "_CRT_SECURE_NO_WARNINGS" into the Preprocessor Definitions white box on the top

#include "framework.h"
#include "LanguageTranslator.h"
#include "ReadWriteFromFile.h"
#include <string>
#include <map>
#include <codecvt>
#include <locale>
#include <vector>

using namespace std;

#define MAX_LOADSTRING 100
#define ADD_WORD 4
#define DELETE_WORD 5
#define TRANSLATE_WORD 6
#define EN_BTN 7
#define BG_BTN 8
#define EDIT_WORD 9

#define FILE "translations.txt"

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

HWND en_dictionaryWord;                         // Dictionary word (add, edit, remove)
HWND bg_dictionaryWord;                         // Dictionary word (add, edit, remove)
HWND hDictionary;                               // Listbox with entire list of words; (eng-bg)
map<string, string> translations;               // Translation words

HWND hTranslatorWindow;                         // Translator window in left panel
HWND hOutputOfTranslatorWindow;
HWND translateBtn;
HWND enToBgBtn;
HWND bgToEnBtn;
HWND enBtn;
HWND bgBtn;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void                addControls(HWND hWnd);
string              HWNDToString(HWND input);
wstring             stringToWstring(const string& str);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LANGUAGETRANSLATOR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LANGUAGETRANSLATOR));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LANGUAGETRANSLATOR));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LANGUAGETRANSLATOR);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   //WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX it s WS_OVERLAPPEDWINDOW without the thickframe (resizable border)
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ReadWriteFromFile rd = ReadWriteFromFile(FILE); //Read from file
   translations = rd.getTranslations(); //Set translations from our file
   //Set to the listbox
   for (auto const& translation : translations) {
       wstring tr = stringToWstring(translation.first + " - " + translation.second);
       SendMessage(hDictionary, LB_ADDSTRING, 0, (LPARAM)tr.c_str());
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE: {
        addControls(hWnd);
    }break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case ADD_WORD: {
                //declare char
                wchar_t en_text[50], bg_text[50], out[101];
         
                GetWindowTextW(bg_dictionaryWord, bg_text, 50);
                GetWindowTextW(en_dictionaryWord, en_text, 50);
                wcscpy_s(out, en_text);
                wcscat_s(out, L" - ");
                wcscat_s(out, bg_text);

                for (auto const& translatedWord : translations) { //Foreach
                    if (translatedWord.first == HWNDToString(en_dictionaryWord)) {
                        MessageBox(hWnd, out, L"This translation is already in our database, try again:", MB_OK);
                        return 0;
                    }
                }

                //Add to translations map
                translations.insert(pair<string, string>(HWNDToString(en_dictionaryWord), HWNDToString(bg_dictionaryWord)));
                //Add to file
                ReadWriteFromFile wf = ReadWriteFromFile();
                wf.addWord(HWNDToString(en_dictionaryWord), HWNDToString(bg_dictionaryWord), FILE);

                //Add the element in the listbox
                SendMessage(hDictionary, LB_ADDSTRING, 0, (LPARAM)out);
                MessageBox(hWnd, out, L"Successfully added to dictionary:", MB_OK);
            
               
            } break;
            case DELETE_WORD: {
                //get current selected item from list box and delete it
                wchar_t selectedItem[255];
                int i = SendMessage(hDictionary, LB_GETCURSEL, 0, 0);
                SendMessage(hDictionary, LB_GETTEXT, i, (LPARAM)selectedItem);

                //Delete the element
                //from the listbox and our map<string,string> translations
                map<string, string>::iterator it; // Declare iterator of our map

                //Convert from wchar_t to wstring
                wstring s(selectedItem);
                //Convert from wstring to string
                string str(s.begin(), s.end());
                //Split by dash('-')
                string key = str.substr(0, str.find(" - "));

                it = translations.find(key); //find in our map
                translations.erase(it); // delete from our map with translations

                // Delete an element from the listbox
                SendMessage(hDictionary, LB_DELETESTRING, i, (LPARAM)selectedItem); 
                 //Delete from a file
                ReadWriteFromFile wf = ReadWriteFromFile(FILE);
                wf.deleteWord(key, FILE);

                MessageBox(hWnd, selectedItem, L"These words are deleted from the dictionary", NULL); // Display on the screen that the element is deleted

            }break;
            case EN_BTN: {
                EnableWindow(bgToEnBtn, false);  // to disable button or other control
                EnableWindow(enToBgBtn, true);  // to enable button or other control
            }break;
            case BG_BTN: {
                EnableWindow(enToBgBtn, false);  // to disable button or other control
                EnableWindow(bgToEnBtn, true);  // to disable button or other control
            }break;
            case TRANSLATE_WORD: {
                if (IsWindowEnabled(enToBgBtn)) {
                    //Clear listbox from previous words
                    SendMessage(hOutputOfTranslatorWindow, LB_RESETCONTENT, 0, 0);

                    //declare char
                    wchar_t en_text[50];

                    GetWindowTextW(hTranslatorWindow, en_text, 50);

                    //Check every word of our map
                    for (auto const& word : translations) {

                        //check by key
                        if (word.first == HWNDToString(hTranslatorWindow)) {
                            //From string to wString
                            wstring wStr = stringToWstring(word.second);
                            SendMessage(hOutputOfTranslatorWindow, LB_ADDSTRING, 0, (LPARAM)wStr.c_str());
                        }
                    }        
                }
                else if (IsWindowEnabled(bgToEnBtn)) {
                    //Clear listbox from previous words
                    SendMessage(hOutputOfTranslatorWindow, LB_RESETCONTENT, 0, 0);

                    //declare char
                    wchar_t bg_text[50];

                    GetWindowTextW(hTranslatorWindow, bg_text, 50);

                    //Check every word of our map
                    for (auto const& word : translations) {

                        //check by key
                        if (word.second == HWNDToString(hTranslatorWindow)) {
                            //From string to wString
                            wstring wStr = stringToWstring(word.first);
                            SendMessage(hOutputOfTranslatorWindow, LB_ADDSTRING, 0, (LPARAM)wStr.c_str());
                        }
                    }
                }
            }break;
            case EDIT_WORD: {
                //declare char
                wchar_t en_text[50], bg_text[50], out[101];

                GetWindowTextW(bg_dictionaryWord, bg_text, 50);
                GetWindowTextW(en_dictionaryWord, en_text, 50);

                wcscpy_s(out, en_text);
                wcscat_s(out, L" - ");
                wcscat_s(out, bg_text);
             
                if (HWNDToString(en_dictionaryWord).length() > 0 && HWNDToString(bg_dictionaryWord).length() > 0) {
                    //get current selected item from list box and update it
                    wchar_t selectedItem[255];
                    int i = SendMessage(hDictionary, LB_GETCURSEL, 0, 0); // -1 unselected item
                    if (i == -1) {
                        MessageBox(hWnd, L"You have to select an item", L"Warning", NULL);
                        return 0;
                    }
                    SendMessage(hDictionary, LB_GETTEXT, i, (LPARAM)selectedItem);

                    //Convert from wchar_t to wstring
                    wstring s(selectedItem);
                    //Convert from wstring to string
                    string str(s.begin(), s.end());
                    //Split by dash('-')
                    string key = str.substr(0, str.find(" - "));
                    map<string, string>::iterator it = translations.find(key);
                    translations.erase(it); // delete from our map with translations

                    //Delete from a file
                    ReadWriteFromFile wf = ReadWriteFromFile(FILE);
                    wf.deleteWord(key, FILE);
                    // Delete an element from the listbox
                    SendMessage(hDictionary, LB_DELETESTRING, i, (LPARAM)selectedItem);

                    //Add to translations map
                    translations.insert(pair<string, string>(HWNDToString(en_dictionaryWord), HWNDToString(bg_dictionaryWord)));
                    //Add to file
                    wf.addWord(HWNDToString(en_dictionaryWord), HWNDToString(bg_dictionaryWord), FILE);
                    //Add the element in the listbox
                    SendMessage(hDictionary, LB_ADDSTRING, 0, (LPARAM)out);
                }
                else MessageBox(hWnd, L"You have to enter english and bulgarian word", L"Warning", NULL);
            }break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // Drawing Text on the right panel buttons
            TextOutA(hdc, 500, 50, "English word: ", 14);
            TextOutA(hdc, 500, 100, "Bulgarian word: ", 15);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

//Add controls like buttons, dialog windows, etc..
void addControls(HWND hWnd) {
    //Left panel
    //en btn
    enToBgBtn = CreateWindowW(L"button", L"English to Bulgarian", WS_VISIBLE | WS_CHILD, 0, 0, 200, 20, hWnd, NULL, NULL, NULL);
    //bg btn
    bgToEnBtn = CreateWindowW(L"button", L"Bulgarian to english", WS_VISIBLE | WS_CHILD, 201, 0, 200, 20, hWnd, NULL, NULL, NULL);

    enBtn = CreateWindowW(L"button", L"EN", WS_VISIBLE | WS_CHILD, 301, 50, 25, 20, hWnd, (HMENU)7, NULL, NULL);

    bgBtn = CreateWindowW(L"button", L"BG", WS_VISIBLE | WS_CHILD, 301, 75, 25, 20, hWnd, (HMENU)8, NULL, NULL);

    hTranslatorWindow = CreateWindowW(L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 0, 21, 300, 200, hWnd, NULL, NULL, NULL);

    translateBtn = CreateWindowW(L"button", L"Translate", WS_VISIBLE | WS_CHILD,  0, 222, 80, 25, hWnd, (HMENU)6, NULL, NULL);

    hOutputOfTranslatorWindow = CreateWindowW(L"ListBox", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 0, 249, 300, 200, hWnd, NULL, NULL, NULL);

    //Right panel
    en_dictionaryWord = CreateWindowW(L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 500, 75, 300, 25, hWnd, NULL, NULL, NULL);

    bg_dictionaryWord = CreateWindowW(L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 500, 125, 300, 25, hWnd, NULL, NULL, NULL);

    CreateWindowW(L"button", L"Add to dictionary", WS_VISIBLE | WS_CHILD | WS_BORDER, 800, 75, 200, 25, hWnd, (HMENU)ADD_WORD, NULL, NULL);

    CreateWindowW(L"button", L"Delete word", WS_VISIBLE | WS_CHILD | WS_BORDER, 800, 100, 200, 25, hWnd, (HMENU)DELETE_WORD, NULL, NULL);

    CreateWindowW(L"button", L"Edit word", WS_VISIBLE | WS_CHILD | WS_BORDER, 800, 125, 200, 25, hWnd, (HMENU)EDIT_WORD, NULL, NULL);

    hDictionary = CreateWindowW(L"ListBox", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 500, 175, 300, 200, hWnd, NULL, NULL, NULL);
}

string HWNDToString(HWND input)
{
    string s;
    int len = GetWindowTextLength(input);
    if (len > 0)
    {
        s.resize(len + 1);
        len = GetWindowTextA(input, &s[0], s.size());
        s.resize(len);
    }
    return s;
}

// utf8toUtf16
wstring stringToWstring(const string& str)
{
    if (str.empty())
        return wstring();

    size_t charsNeeded = ::MultiByteToWideChar(CP_UTF8, 0,
        str.data(), (int)str.size(), NULL, 0);
    if (charsNeeded == 0)
        throw runtime_error("Failed converting UTF-8 string to UTF-16");

    vector<wchar_t> buffer(charsNeeded);
    int charsConverted = ::MultiByteToWideChar(CP_UTF8, 0,
        str.data(), (int)str.size(), &buffer[0], buffer.size());
    if (charsConverted == 0)
        throw runtime_error("Failed converting UTF-8 string to UTF-16");

    return wstring(&buffer[0], charsConverted);
}
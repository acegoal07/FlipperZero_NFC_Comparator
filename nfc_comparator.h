#pragma once
// #ifndef NFC_COMPARATOR_H
// #define NFC_COMPARATOR_H

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <gui/modules/file_browser.h>
#include <gui/modules/popup.h>
#include <gui/modules/widget.h>
#include <notification/notification_messages.h>
#include <nfc_device.h>
#include <nfc_listener.h>
#include <nfc_scanner.h>

#include "nfc_comparator_icons.h"
#include "scenes/nfc_comparator_scene.h"
#include "lib/reader_worker/nfc_comparator_reader_worker.h"
#include "lib/led_worker/nfc_comparator_led_worker.h"

#define NFC_ITEM_LOCATION "/ext/nfc/"

/** Enum for all possible views in the NFC Comparator app */
typedef enum {
   NfcComparatorView_Submenu,
   NfcComparatorView_FileBrowser,
   NfcComparatorView_Popup,
   NfcComparatorView_Widget,
   NfcComparatorView_Count
} NfcComparatorViews;

/** File browser view and its output string */
typedef struct {
   FileBrowser* view;
   FuriString* output;
   FuriString* tmp_output;
} NfcComparatorFileBrowserView;

/** All views used by the NFC Comparator app */
typedef struct {
   Submenu* submenu;
   NfcComparatorFileBrowserView file_browser;
   Popup* popup;
   Widget* widget;
} NfcComparatorView;

/** Worker struct for NFC comparison logic */
typedef struct {
   NfcComparatorReaderWorker* nfc_comparator_reader_worker;
   NfcComparatorReaderWorkerCompareChecks compare_checks;
} NfcComparatorWorker;

/** Main app struct holding all state */
typedef struct {
   SceneManager* scene_manager;
   ViewDispatcher* view_dispatcher;
   NotificationApp* notification_app;
   NfcComparatorView views;
   NfcComparatorWorker worker;
} NfcComparator;

// #endif // NFC_COMPARATOR_H

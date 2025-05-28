#include "../nfc_comparator.h"

typedef enum {
   NfcComparatorMainMenu_PhysicalScan,
   NfcComparatorMainMenu_DigitalScan,
   NfcComparatorMainMenu_SelectNfcCard
} NfcComparatorMainMenuMenuSelection;

static void nfc_comparator_main_menu_menu_callback(void* context, uint32_t index) {
   furi_assert(context);
   NfcComparator* nfc_comparator = context;
   scene_manager_handle_custom_event(nfc_comparator->scene_manager, index);
}

void nfc_comparator_main_menu_scene_on_enter(void* context) {
   furi_assert(context);
   NfcComparator* nfc_comparator = context;

   FuriString* header = furi_string_alloc_printf("NFC Comparator v%s", FAP_VERSION);
   submenu_set_header(nfc_comparator->views.submenu, furi_string_get_cstr(header));
   furi_string_free(header);

   submenu_add_lockable_item(
      nfc_comparator->views.submenu,
      "Start Physical Comparator",
      NfcComparatorMainMenu_PhysicalScan,
      nfc_comparator_main_menu_menu_callback,
      nfc_comparator,
      furi_string_empty(nfc_comparator->views.file_browser.output),
      "No NFC\ncard selected");

   submenu_add_lockable_item(
      nfc_comparator->views.submenu,
      "Start Digital Comparator",
      NfcComparatorMainMenu_DigitalScan,
      nfc_comparator_main_menu_menu_callback,
      nfc_comparator,
      furi_string_empty(nfc_comparator->views.file_browser.output),
      "No NFC\ncard selected");

   submenu_add_item(
      nfc_comparator->views.submenu,
      "Select NFC Card",
      NfcComparatorMainMenu_SelectNfcCard,
      nfc_comparator_main_menu_menu_callback,
      nfc_comparator);

   submenu_add_item(nfc_comparator->views.submenu, "By acegoal07", 0, NULL, NULL);

   view_dispatcher_switch_to_view(nfc_comparator->view_dispatcher, NfcComparatorView_Submenu);
}

bool nfc_comparator_main_menu_scene_on_event(void* context, SceneManagerEvent event) {
   furi_assert(context);
   NfcComparator* nfc_comparator = context;
   bool consumed = false;
   if(event.type == SceneManagerEventTypeCustom) {
      switch(event.event) {
      case NfcComparatorMainMenu_PhysicalScan:
         scene_manager_next_scene(nfc_comparator->scene_manager, NfcComparatorScene_PhysicalScan);
         consumed = true;
         break;
      case NfcComparatorMainMenu_DigitalScan:
         scene_manager_next_scene(nfc_comparator->scene_manager, NfcComparatorScene_DigitalScan);
         consumed = true;
         break;
      case NfcComparatorMainMenu_SelectNfcCard:
         scene_manager_next_scene(nfc_comparator->scene_manager, NfcComparatorScene_SelectNfcCard);
         consumed = true;
         break;
      default:
         break;
      }
   }
   return consumed;
}

void nfc_comparator_main_menu_scene_on_exit(void* context) {
   furi_assert(context);
   NfcComparator* nfc_comparator = context;
   submenu_reset(nfc_comparator->views.submenu);
}

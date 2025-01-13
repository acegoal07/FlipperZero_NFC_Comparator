
#include "../nfc_comparator.h"

typedef enum {
   NfcComparatorMainMenu_Start,
   NfcComparatorMainMenu_SelectNfcCard
} NfcComparatorMainMenuMenuSelection;

void nfc_comparator_main_menu_menu_callback(void* context, uint32_t index) {
   NfcComparator* nfc_comparator = context;
   scene_manager_handle_custom_event(nfc_comparator->scene_manager, index);
}

void nfc_comparator_main_menu_scene_on_enter(void* context) {
   NfcComparator* nfc_comparator = context;

   FuriString* header = furi_string_alloc_printf("NFC Comparator v%s", FAP_VERSION);
   submenu_set_header(nfc_comparator->submenu, furi_string_get_cstr(header));
   furi_string_free(header);

   submenu_add_lockable_item(
      nfc_comparator->submenu,
      "Start",
      NfcComparatorMainMenu_Start,
      nfc_comparator_main_menu_menu_callback,
      nfc_comparator,
      !nfc_comparator->loaded_nfc_card,
      "No NFC card selected");

   submenu_add_item(
      nfc_comparator->submenu,
      "Select NFC Card",
      NfcComparatorMainMenu_SelectNfcCard,
      nfc_comparator_main_menu_menu_callback,
      nfc_comparator);

   view_dispatcher_switch_to_view(nfc_comparator->view_dispatcher, NfcComparatorView_Submenu);
}

bool nfc_comparator_main_menu_scene_on_event(void* context, SceneManagerEvent event) {
   NfcComparator* nfc_comparator = context;
   UNUSED(nfc_comparator);
   bool consumed = false;
   if(event.type == SceneManagerEventTypeCustom) {
      switch(event.event) {
      // case NfcComparatorMainMenu_Start:
      //    scene_manager_next_scene(nfc_comparator->scene_manager, NfcComparatorScene_Emulation);
      //    consumed = true;
      //    break;
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
   NfcComparator* nfc_comparator = context;
   submenu_reset(nfc_comparator->submenu);
}

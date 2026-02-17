#include "../nfc_comparator.h"

static const char* EmvFieldNames[] = {
   "Card Number",
   "Cardholder Name",
   "Expiration Date",
   "AID",
   "Application Name",
   "Application Label",
   "Country Code",
   "Currency Code"};

void nfc_comparator_data_breakdown_scene_on_enter(void* context) {
   furi_assert(context);
   NfcComparator* nfc_comparator = context;

   furi_string_reset(nfc_comparator->views.text_box.store);

   uint16_t total = nfc_comparator->workers.compare_checks->diff.total;
   uint16_t diff = nfc_comparator->workers.compare_checks->diff.count;
   uint16_t similar = total - diff;
   uint8_t percentage = (similar * 100) / total;

   const char* unit_name;
   switch(nfc_comparator->workers.compare_checks->diff.unit) {
   case NfcCompareChecksComparedDataType_Blocks:
      unit_name = "blocks";
      break;
   case NfcCompareChecksComparedDataType_Pages:
      unit_name = "pages";
      break;
   case NfcCompareChecksComparedDataType_Bytes:
      unit_name = "bytes";
      break;
   case NfcCompareChecksComparedDataType_EmvFields:
      unit_name = "fields";
      break;
   default:
      unit_name = "units";
      break;
   }

   furi_string_cat_printf(
      nfc_comparator->views.text_box.store,
      "Similarity:\n%u%% (%u/%u %s match)\n\nDifferent %s (%u):\n",
      percentage,
      similar,
      total,
      unit_name,
      unit_name,
      diff);

   for(int i = 0; i < diff; i++) {
      const uint16_t* idx =
         simple_array_cget(nfc_comparator->workers.compare_checks->diff.indices, i);

      if(nfc_comparator->workers.compare_checks->diff.unit ==
         NfcCompareChecksComparedDataType_EmvFields) {
         furi_string_cat_printf(nfc_comparator->views.text_box.store, "%s", EmvFieldNames[*idx]);

         if(i < diff - 1) {
            furi_string_cat(nfc_comparator->views.text_box.store, ",\n");
         }
      } else {
         furi_string_cat_printf(nfc_comparator->views.text_box.store, "%d", *idx);

         if(i < diff - 1) {
            furi_string_cat(nfc_comparator->views.text_box.store, ", ");
            if((i + 1) % 5 == 0 && nfc_comparator->workers.compare_checks->diff.unit !=
                                      NfcCompareChecksComparedDataType_EmvFields) {
               furi_string_cat(nfc_comparator->views.text_box.store, "\n");
            }
         }
      }
   }

   text_box_reset(nfc_comparator->views.text_box.view);
   text_box_set_text(
      nfc_comparator->views.text_box.view,
      furi_string_get_cstr(nfc_comparator->views.text_box.store));
   text_box_set_font(nfc_comparator->views.text_box.view, TextBoxFontText);

   view_dispatcher_switch_to_view(nfc_comparator->view_dispatcher, NfcComparatorView_TextBox);
}

bool nfc_comparator_data_breakdown_scene_on_event(void* context, SceneManagerEvent event) {
   UNUSED(context);
   UNUSED(event);
   return false;
}

void nfc_comparator_data_breakdown_scene_on_exit(void* context) {
   furi_assert(context);
   NfcComparator* nfc_comparator = context;
   text_box_reset(nfc_comparator->views.text_box.view);
   furi_string_reset(nfc_comparator->views.text_box.store);
}

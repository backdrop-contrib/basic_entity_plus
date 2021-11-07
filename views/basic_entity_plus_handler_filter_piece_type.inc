<?php

/**
 * @file
 * Definition of basic_entity_plus_handler_filter_basic_entity_plus_type.
 */

/**
 * Filter by basic_entity_plus type.
 *
 * @ingroup views_filter_handlers
 */
class basic_entity_plus_handler_filter_basic_entity_plus_type extends views_handler_filter_in_operator {
  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Family types');
      $types = basic_entity_plus_types();
      $options = array();
      foreach ($types as $type => $info) {
        $options[$type] = t($info->label);
      }
      asort($options);
      $this->value_options = $options;
    }
  }
}

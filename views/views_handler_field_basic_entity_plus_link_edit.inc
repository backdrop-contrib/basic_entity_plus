<?php

/**
 * @file
 * Definition of views_handler_field_basic_entity_plus_link_edit.
 */

/**
 * Field handler to present an edit link to user
 *
 * @ingroup views_field_handlers
 */
class views_handler_field_basic_entity_plus_link_edit extends views_handler_field_basic_entity_plus_link {

  /**
   * {@inheritdoc}
   */
  public function render_link($data, $values) {
    if ($data && user_access('edit any basic_entity_plus entities')) {
      $this->options['alter']['make_link'] = TRUE;

      $text = !empty($this->options['text']) ? $this->options['text'] : t('edit');

      $this->options['alter']['path'] = "basic_entity_plus/$data/edit";
      $this->options['alter']['query'] = backdrop_get_destination();

      return $text;
    }
  }

}

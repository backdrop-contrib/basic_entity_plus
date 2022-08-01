<?php
/**
 * @file
 * Provide views data and handlers for node.module.
 *
 * @ingroup views_module_handlers
 */

/**
 * Implements hook_views_data().
 * 
 * Defines custom views field handlers to view, edit and delete. 
 */
function basic_entity_plus_views_data_alter(&$data) {

  $data['basic_entity_plus']['view_basic_entity_plus'] = array(
    'field' => array(
      'title' => t('Link to basic_entity_plus'),
      'help' => t('Provide a simple link to the basic_entity_plus.'),
      'handler' => 'views_handler_field_basic_entity_plus_link',
    ),
  );
  
  $data['basic_entity_plus']['edit_basic_entity_plus'] = array(
    'field' => array(
      'title' => t('Link to edit basic_entity_plus'),
      'help' => t('Provide a simple link to edit the basic_entity_plus.'),
      'handler' => 'views_handler_field_basic_entity_plus_link_edit',
    ),
  );

  $data['basic_entity_plus']['delete_basic_entity_plus'] = array(
    'field' => array(
      'title' => t('Link to delete basic_entity_plus'),
      'help' => t('Provide a simple link to delete the basic_entity_plus.'),
      'handler' => 'views_handler_field_basic_entity_plus_link_delete',
    ),
  );
}

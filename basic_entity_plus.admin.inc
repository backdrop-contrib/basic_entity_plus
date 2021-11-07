<?php

/**
 * Generates the basic_entity_plus type editing form.
 */
function basic_entity_plus_type_form($form, &$form_state, $basic_entity_plus_type, $op = 'edit') {

  if ($op == 'clone') {
    $basic_entity_plus_type->label .= ' (cloned)';
    $basic_entity_plus_type->type = '';
  }

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $basic_entity_plus_type->label,
    '#description' => t('The human-readable name of this basic_entity_plus type.'),
    '#required' => TRUE,
    '#size' => 30,
  );

  // Machine-readable type name.
  $form['type'] = array(
    '#type' => 'machine_name',
    '#default_value' => isset($basic_entity_plus_type->type) ? $basic_entity_plus_type->type : '',
    '#maxlength' => 32,
    '#disabled' => $basic_entity_plus_type->isLocked() && $op != 'clone',
    '#machine_name' => array(
      'exists' => 'basic_entity_plus_types',
      'source' => array('label'),
    ),
    '#description' => t('A unique machine-readable name for this basic_entity_plus type. It must only contain lowercase letters, numbers, and underscores.'),
  );

  $form['description'] = array(
    '#type' => 'textarea',
    '#default_value' => isset($basic_entity_plus_type->description) ? $basic_entity_plus_type->description : '',
    '#description' => t('Description about the basic_entity_plus type.'),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save basic_entity_plus type'),
    '#weight' => 40,
  );

  if (!$basic_entity_plus_type->isLocked() && $op != 'add' && $op != 'clone') {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete basic_entity_plus type'),
      '#weight' => 45,
      '#limit_validation_errors' => array(),
      '#submit' => array('basic_entity_plus_type_form_submit_delete')
    );
  }
  return $form;
}

/**
 * Submit handler for creating/editing basic_entity_plus_type.
 */
function basic_entity_plus_type_form_submit(&$form, &$form_state) {
  $basic_entity_plus_type = entity_ui_form_submit_build_entity($form, $form_state);
  // Save and go back.
  $op = basic_entity_plus_type_save($basic_entity_plus_type);
  if ($op == SAVED_NEW) {
    // Notify Field API of new bundle
    field_attach_create_bundle('basic_entity_plus', $basic_entity_plus_type->type);
  }

  // Redirect user back to list of basic_entity_plus types.
  $form_state['redirect'] = 'admin/structure/basic_entity_plus-types';
}

function basic_entity_plus_type_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/basic_entity_plus-types/' . $form_state['basic_entity_plus_type']->type . '/delete';
}

/**
 * Order type delete form.
 */
function basic_entity_plus_type_form_delete_confirm($form, &$form_state, $basic_entity_plus_type) {
  $form_state['basic_entity_plus_type'] = $basic_entity_plus_type;
  // Always provide entity id in the same form key as in the entity edit form.
  $form['basic_entity_plus_type_id'] = array('#type' => 'value', '#value' => entity_plus_id('basic_entity_plus_type',$basic_entity_plus_type));
  return confirm_form($form,
    t('Are you sure you want to delete basic_entity_plus type %title?', array('%title' => entity_label('basic_entity_plus_type', $basic_entity_plus_type))),
    'basic_entity_plus/' . entity_plus_id('basic_entity_plus_type' ,$basic_entity_plus_type),
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Order type delete form submit handler.
 */
function basic_entity_plus_type_form_delete_confirm_submit($form, &$form_state) {
  $basic_entity_plus_type = $form_state['basic_entity_plus_type'];
  basic_entity_plus_type_delete($basic_entity_plus_type);

  watchdog('basic_entity_plus_type', '@type: deleted %title.', array('@type' => $basic_entity_plus_type->type, '%title' => $basic_entity_plus_type->label));
  backdrop_set_message(t('@type %title has been deleted.', array('@type' => $basic_entity_plus_type->type, '%title' => $basic_entity_plus_type->label)));

  $form_state['redirect'] = 'admin/structure/basic_entity_plus-types';
}

/**
 * Page to select basic_entity_plus Type to add new basic_entity_plus.
 */
function basic_entity_plus_admin_add_page() {
  $items = array();
  foreach (basic_entity_plus_types() as $basic_entity_plus_type_key => $basic_entity_plus_type) {
    $items[] = l(entity_label('basic_entity_plus_type', $basic_entity_plus_type), 'basic_entity_plus/add/' . $basic_entity_plus_type_key);
  } 
  return array('list' => array('#theme' => 'item_list', '#items' => $items, '#title' => t('Select type of Basic Entity Plus to create.')));
}

/**
 * Add new basic_entity_plus page callback.
 */
function basic_entity_plus_add($type) {
  $basic_entity_plus_type = basic_entity_plus_types($type);

  $basic_entity_plus = entity_create('basic_entity_plus', array('type' => $type));
  backdrop_set_title(t('Create @name', array('@name' => entity_label('basic_entity_plus_type', $basic_entity_plus_type))));

  $output = backdrop_get_form('basic_entity_plus_form', $basic_entity_plus);

  return $output;
}

/**
 * Order Form.
 */
function basic_entity_plus_form($form, &$form_state, $basic_entity_plus) {
  $form_state['basic_entity_plus'] = $basic_entity_plus;

  $form['title'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('Title'),
    '#default_value' => $basic_entity_plus->title,
  );

  $form['uid'] = array(
    '#type' => 'value',
    '#value' => $basic_entity_plus->uid,
  );

  $form['type'] = array(
    '#type' => 'value',
    '#value' => $basic_entity_plus->type,
  );

  if (!(isset($basic_entity_plus->is_new) && $basic_entity_plus->is_new)) {
    $form['basic_entity_plus_id'] = array(
      '#type' => 'value',
      '#value' => $basic_entity_plus->basic_entity_plus_id,
    );
  }
  
  field_attach_form('basic_entity_plus', $basic_entity_plus, $form, $form_state);
  
  $submit = array();
  if (!empty($form['#submit'])) {
    $submit += $form['#submit'];
  }

  $form['actions'] = array(
    '#weight' => 100,
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Basic Entity Plus'),
    '#submit' => $submit + array('basic_entity_plus_form_submit'),
  );

  // Show Delete button if we edit basic_entity_plus.
  $basic_entity_plus_id = entity_plus_id('basic_entity_plus', $basic_entity_plus);
  if (!empty($basic_entity_plus_id) && basic_entity_plus_access('edit', $basic_entity_plus)) {
    // Show the Delete button only if basic_entity_plus does not contain students
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#submit' => array('basic_entity_plus_form_submit_delete'),
    );      
  }

  $form['#validate'][] = 'basic_entity_plus_form_validate';

  return $form;
}

function basic_entity_plus_form_validate($form, &$form_state) {
  entity_form_field_validate('basic_entity_plus', $form, $form_state);
}

/**
 * Order submit handler.
 */
function basic_entity_plus_form_submit($form, &$form_state) {
  $basic_entity_plus = $form_state['basic_entity_plus'];

  entity_form_submit_build_entity('basic_entity_plus', $basic_entity_plus, $form, $form_state);

  basic_entity_plus_save($basic_entity_plus);

  $basic_entity_plus_uri = basic_entity_plus_uri('basic_entity_plus', $basic_entity_plus);

  $form_state['redirect'] = $basic_entity_plus_uri['path'];

  backdrop_set_message(t('Basic Entity %title saved.', array('%title' => entity_label('basic_entity_plus', $basic_entity_plus))));
}

function basic_entity_plus_form_submit_delete($form, &$form_state) {
  $basic_entity_plus = $form_state['basic_entity_plus'];
  $basic_entity_plus_uri = basic_entity_plus_uri('basic_entity_plus', $basic_entity_plus);
  $form_state['redirect'] = $basic_entity_plus_uri['path'] . '/delete';
}

/**
 * Delete confirmation form.
 */
function basic_entity_plus_delete_form($form, &$form_state, $basic_entity_plus) {
  $form_state['basic_entity_plus'] = $basic_entity_plus;
  // Always provide entity id in the same form key as in the entity edit form.
  $form['basic_entity_plus_type_id'] = array('#type' => 'value', '#value' => entity_plus_id('basic_entity_plus' ,$basic_entity_plus));
  $basic_entity_plus_uri = basic_entity_plus_uri('basic_entity_plus', $basic_entity_plus);
  return confirm_form($form,
    t('Are you sure you want to delete basic_entity_plus %title?', array('%title' => entity_label('basic_entity_plus', $basic_entity_plus))),
    $basic_entity_plus_uri['path'],
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Delete form submit handler.
 */
function basic_entity_plus_delete_form_submit($form, &$form_state) {
  $basic_entity_plus = $form_state['basic_entity_plus'];
  basic_entity_plus_delete($basic_entity_plus);

  backdrop_set_message(t('Basic Entity %title deleted.', array('%title' => entity_label('basic_entity_plus', $basic_entity_plus))));

  $form_state['redirect'] = '<front>';
}

/**
 * Delete confirmation for multiple basic_entities through action and bulk operation
 */
function basic_entity_plus_multiple_delete_confirm($form, &$form_state) {
  if (isset($form_state['basic_entity_plus_ids'])) {
    $basic_entity_plus_ids = $form_state['basic_entity_plus_ids'];
  }
  elseif (isset($_SESSION['basic_entity_plus_delete_action']['timestamp']) && (REQUEST_TIME - $_SESSION['basic_entity_plus_delete_action']['timestamp'] < 6000)) {
    $basic_entity_plus_ids = $_SESSION['basic_entity_plus_delete_action']['basic_entity_plus_ids'];
    $form_state['basic_entity_plus_ids'] = $basic_entity_plus_ids;
    $form_state['cache'] = TRUE;
    unset($_SESSION['basic_entity_plus_delete_action']);
  }
  else {
    $basic_entity_plus_ids = array();
  }

  $form['#tree'] = TRUE;

  if (empty($basic_entity_plus_ids)) {
    $destination = isset($_GET['destination']) ? $_GET['destination'] : '<front>';
    $form['empty']['#markup'] = '<p>' . t('Return to the <a href="!url">front page</a>.', array('!url' => url($destination))) . '</p>';
    backdrop_set_message(t('No Basic Entities have been selected for deletion.'), 'error');
    return $form;
  }

  $form['basic_entity_plus_list'] = array(
    '#theme' => 'item_list',
    '#items' => array(),
  );

  $rows = db_query('SELECT basic_entity_plus_id, title FROM {basic_entity_plus} WHERE basic_entity_plus_id IN (:basic_entity_plus_ids)', array(':basic_entity_plus_ids' => $basic_entity_plus_ids))->fetchAllKeyed();
  foreach ($rows as $basic_entity_plus_id => $title) {
    $form['basic_entities'][$basic_entity_plus_id] = array(
      '#type' => 'hidden',
      '#value' => $basic_entity_plus_id,
    );
    $form['basic_entity_plus_list']['#items'][] = check_plain($title);
  }

  $confirm_question = format_plural(count($rows), 'Are you sure you want to delete this item?', 'Are you sure you want to delete these items?');
  return confirm_form($form, $confirm_question, '<front>', t('This action cannot be undone.'), t('Delete'), t('Cancel'));
}

/**
 * Form submission handler for basic_entity_plus_multiple_delete_confirm().
 */
function basic_entity_plus_multiple_delete_confirm_submit($form, &$form_state) {
  basic_entity_plus_delete_multiple(array_keys($form_state['values']['basic_entities']));
  cache_clear_all();
  $count = count($form_state['values']['basic_entities']);
  watchdog('basic_entity_plus', 'Deleted @count basic entities.', array('@count' => $count));

  backdrop_set_message(format_plural($count, 'Deleted 1 basic_entity_plus.', 'Deleted @count basic entities.'));
  $form_state['redirect'] = '<front>';
}

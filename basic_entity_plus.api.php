<?php
/**
 * @file
 * Hooks provided by this module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Acts on basic_entity_plus being loaded from the database.
 *
 * This hook is invoked during $basic_entity_plus loading, which is handled by
 * entity_load(), via the EntityCRUDController.
 *
 * @param array $entities
 *   An array of $basic_entity_plus entities being loaded, keyed by id.
 *
 * @see hook_entity_load()
 */
function hook_basic_entity_plus_load(array $entities) {
  $result = db_query('SELECT pid, foo FROM {mytable} WHERE pid IN(:ids)', array(':ids' => array_keys($entities)));
  foreach ($result as $record) {
    $entities[$record->pid]->foo = $record->foo;
  }
}

/**
 * Responds when a $basic_entity_plus is inserted.
 *
 * This hook is invoked after the $basic_entity_plus is inserted into the database.
 *
 * @param Family $basic_entity_plus
 *   The $basic_entity_plus that is being inserted.
 *
 * @see hook_entity_insert()
 */
function hook_basic_entity_plus_insert(BasicEntityPlus $basic_entity_plus) {
  db_insert('mytable')
    ->fields(array(
      'id' => entity_plus_id('basic_entity_plus', $basic_entity_plus),
      'extra' => print_r($basic_entity_plus, TRUE),
    ))
    ->execute();
}

/**
 * Acts on a $basic_entity_plus being inserted or updated.
 *
 * This hook is invoked before the $basic_entity_plus is saved to the database.
 *
 * @param Family $basic_entity_plus
 *   The $basic_entity_plus that is being inserted or updated.
 *
 * @see hook_entity_presave()
 */
function hook_basic_entity_plus_presave(BasicEntityPlus $basic_entity_plus) {
  $basic_entity_plus->name = 'foo';
}

/**
 * Responds to a $basic_entity_plus being updated.
 *
 * This hook is invoked after the $basic_entity_plus has been updated in the database.
 *
 * @param Family $basic_entity_plus
 *   The $basic_entity_plus that is being updated.
 *
 * @see hook_entity_update()
 */
function hook_basic_entity_plus_update(BasicEntityPlus $basic_entity_plus) {
  db_update('mytable')
    ->fields(array('extra' => print_r($basic_entity_plus, TRUE)))
    ->condition('id', entity_plus_id('basic_entity_plus', $basic_entity_plus))
    ->execute();
}

/**
 * Responds to $basic_entity_plus deletion.
 *
 * This hook is invoked after the $basic_entity_plus has been removed from the database.
 *
 * @param Family $basic_entity_plus
 *   The $basic_entity_plus that is being deleted.
 *
 * @see hook_entity_delete()
 */
function hook_basic_entity_plus_delete(BasicEntityPlus $basic_entity_plus) {
  db_delete('mytable')
    ->condition('pid', entity_plus_id('basic_entity_plus', $basic_entity_plus))
    ->execute();
}

/**
 * Act on a basic_entity_plus that is being assembled before rendering.
 *
 * @param $basic_entity_plus
 *   The basic_entity_plus entity.
 * @param $view_mode
 *   The view mode the basic_entity_plus is rendered in.
 * @param $langcode
 *   The language code used for rendering.
 *
 * The module may add elements to $basic_entity_plus->content prior to rendering. The
 * structure of $basic_entity_plus->content is a renderable array as expected by
 * drupal_render().
 *
 * @see hook_entity_prepare_view()
 * @see hook_entity_view()
 */
function hook_basic_entity_plus_view($basic_entity_plus, $view_mode, $langcode) {
  $basic_entity_plus->content['my_additional_field'] = array(
    '#markup' => $additional_field,
    '#weight' => 10,
    '#theme' => 'mymodule_my_additional_field',
  );
}

/**
 * Alter the results of entity_view() for basic_entities.
 *
 * @param $build
 *   A renderable array representing the basic_entity_plus content.
 *
 * This hook is called after the content has been assembled in a structured
 * array and may be used for doing processing which requires that the complete
 * basic_entity_plus content structure has been built.
 *
 * If the module wishes to act on the rendered HTML of the basic_entity_plus rather than
 * the structured content array, it may use this hook to add a #post_render
 * callback. Alternatively, it could also implement hook_preprocess_basic_entity_plus().
 * See drupal_render() and theme() documentation respectively for details.
 *
 * @see hook_entity_view_alter()
 */
function hook_basic_entity_plus_view_alter($build) {
  if ($build['#view_mode'] == 'full' && isset($build['an_additional_field'])) {
    // Change its weight.
    $build['an_additional_field']['#weight'] = -10;

    // Add a #post_render callback to act on the rendered HTML of the entity.
    $build['#post_render'][] = 'my_module_post_render';
  }
}

/**
 * Acts on basic_entity_plus_type being loaded from the database.
 *
 * This hook is invoked during basic_entity_plus_type loading, which is handled by
 * entity_load(), via the EntityCRUDController.
 *
 * @param array $entities
 *   An array of basic_entity_plus_type entities being loaded, keyed by id.
 *
 * @see hook_entity_load()
 */
function hook_basic_entity_plus_type_load(array $entities) {
  $result = db_query('SELECT pid, foo FROM {mytable} WHERE pid IN(:ids)', array(':ids' => array_keys($entities)));
  foreach ($result as $record) {
    $entities[$record->pid]->foo = $record->foo;
  }
}

/**
 * Responds when a basic_entity_plus_type is inserted.
 *
 * This hook is invoked after the basic_entity_plus_type is inserted into the database.
 *
 * @param FamilyType $basic_entity_plus_type
 *   The basic_entity_plus_type that is being inserted.
 *
 * @see hook_entity_insert()
 */
function hook_basic_entity_plus_type_insert(BasicEntityPlus $basic_entity_plus_type) {
  db_insert('mytable')
    ->fields(array(
      'id' => entity_plus_id('basic_entity_plus_type', $basic_entity_plus_type),
      'extra' => print_r($basic_entity_plus_type, TRUE),
    ))
    ->execute();
}

/**
 * Acts on a basic_entity_plus_type being inserted or updated.
 *
 * This hook is invoked before the basic_entity_plus_type is saved to the database.
 *
 * @param FamilyType $basic_entity_plus_type
 *   The basic_entity_plus_type that is being inserted or updated.
 *
 * @see hook_entity_presave()
 */
function hook_basic_entity_plus_type_presave(BasicEntityPlus $basic_entity_plus_type) {
  $basic_entity_plus_type->name = 'foo';
}

/**
 * Responds to a basic_entity_plus_type being updated.
 *
 * This hook is invoked after the basic_entity_plus_type has been updated in the database.
 *
 * @param FamilyType $basic_entity_plus_type
 *   The basic_entity_plus_type that is being updated.
 *
 * @see hook_entity_update()
 */
function hook_basic_entity_plus_type_update(BasicEntityPlus $basic_entity_plus_type) {
  db_update('mytable')
    ->fields(array('extra' => print_r($basic_entity_plus_type, TRUE)))
    ->condition('id', entity_plus_id('basic_entity_plus_type', $basic_entity_plus_type))
    ->execute();
}

/**
 * Responds to basic_entity_plus_type deletion.
 *
 * This hook is invoked after the basic_entity_plus_type has been removed from the database.
 *
 * @param FamilyType $basic_entity_plus_type
 *   The basic_entity_plus_type that is being deleted.
 *
 * @see hook_entity_delete()
 */
function hook_basic_entity_plus_type_delete(BasicEntityPlus $basic_entity_plus_type) {
  db_delete('mytable')
    ->condition('pid', entity_plus_id('basic_entity_plus_type', $basic_entity_plus_type))
    ->execute();
}

/**
 * Define default basic_entity_plus_type configurations.
 *
 * @return
 *   An array of default basic_entity_plus_type, keyed by machine names.
 *
 * @see hook_default_basic_entity_plus_type_alter()
 */
function hook_default_basic_entity_plus_type() {
  $defaults['main'] = entity_create('basic_entity_plus_type', array(
    // …
  ));
  return $defaults;
}

/**
 * Alter default basic_entity_plus_type configurations.
 *
 * @param array $defaults
 *   An array of default basic_entity_plus_type, keyed by machine names.
 *
 * @see hook_default_basic_entity_plus_type()
 */
function hook_default_basic_entity_plus_type_alter(array &$defaults) {
  $defaults['main']->name = 'custom name';
}

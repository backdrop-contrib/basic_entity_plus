<?php

class BasicEntityPlusController extends EntityPlusController {

  /**
   * This method is called when we use entity_create(). It provides defaults
   * for the entity properties. 
   */
  public function create(array $values = array()) {
    global $user;
    $values += array(
      'title' => '',
      'created' => REQUEST_TIME,
      'changed' => REQUEST_TIME,
      'uid' => $user->uid,
    );
    return parent::create($values);
  }

  /** 
   * This method is called when using Entity::view() or entity_view(). It builds
   * the render array. We provide 
   */
  public function buildContent($entity, $view_mode = 'full', $langcode = NULL, $content = array()) {
    $wrapper = entity_metadata_wrapper('basic_entity_plus', $entity);
    
    $build = parent::buildContent($entity, $view_mode, $langcode, $content);

    // Since we defined 'extra field controller class' in hook_entity_info for
    // these entities, the theming code below is not needed. 
    // If uncommented, it would theme the properties of the entity as fields.

    // $field_defaults = array(
    //   '#theme' => 'field',
    //   '#access' => TRUE,
    //   '#label_display' => 'inline',
    //   '#view_mode' => $view_mode,
    //   '#language' => $langcode,
    //   '#field_type' => 'text',
    //   '#entity_type' => 'basic_entity_plus',
    //   '#bundle' => $entity->type,
    //   '#formatter' => 'text_default',
    // );

    // $build['title'] = array(
    //   '#title' => t('Title'),
    //   '#field_name' => 'field_fake_title',
    //   '#items' => array(array('value' => $wrapper->label())),
    //   0 => array('#markup' => $wrapper->label())
    // ) + $field_defaults;

    // $build['created'] = array(
    //   '#title' => t('Created'),
    //   '#field_name' => 'field_fake_created',
    //   '#items' => array(array('value' => format_date($wrapper->created->value(),'medium'))),
    //   0 => array('#markup' => format_date($wrapper->created->value(),'medium')),
    // ) + $field_defaults;

    return $build;
  }
}

class BasicEntityPlusTypeController extends EntityPlusControllerExportable {
  
  /**
   * This method is called when we use entity_create(). It provides defaults
   * for the entity properties. 
   */
   public function create(array $values = array()) {
    $values += array(
      'label' => '',
      'description' => '',
    );
    return parent::create($values);
  }

  /**
   * Save BasicEntityPlus Type.
   */
  public function save($entity, DatabaseTransaction $transaction = NULL) {
    // Rebuild menu registry. We do not call menu_rebuild directly, but set
    // state that indicates rebuild in the end.
    state_set('menu_rebuild_needed', TRUE);
    return parent::save($entity, $transaction);
  }
}

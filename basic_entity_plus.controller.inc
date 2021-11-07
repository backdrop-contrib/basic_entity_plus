<?php

/**
 * Defines a basic entity plus class.
 * 
 * This class extends the core Entity class. It must define many of the undefined 
 * methods in the original class and interface
 */
class BasicEntityPlus extends Entity implements EntityInterface {
  protected $entityType; // stores the entity type
  protected $entityInfo; // stores the info provided in hook_entity_info() for this entity
  protected $idKey, $nameKey, $statusKey; // Various key names for storing id, name and status
  protected $defaultLabel = FALSE;
  protected $wrapper; // stores the entitiy_metadata_wrapper for this entity

  /**
   * Overrides Entity construct method to accept entityType. Eventually all this has to be moved to a higher class!
   */
  public function __construct(array $values = array(), $entityType = NULL) {
    if (empty($entityType)) {
      throw new Exception('Cannot create an instance of Entity without a specified entity type.');
    }
    $this->entityType = $entityType;
    $this->setUp();
    // Set initial values.
    foreach ($values as $key => $value) {
      $this->$key = $value;
    }
  }

  /**
   * Set up the object instance on construction or unserializiation. Move to higher class
   */
  protected function setUp() {
    $this->entityInfo = entity_get_info($this->entityType);
    $this->idKey = $this->entityInfo['entity keys']['id'];
    // ['entity keys']['name'] is only used for configuration entities that store the machine name of the bundle they define. 
    // For content entities, this is the same as idKey 
    $this->nameKey = isset($this->entityInfo['entity keys']['name']) ? $this->entityInfo['entity keys']['name'] : $this->idKey;
    // ['entity keys']['status'] is only used for configuration entities
    $this->statusKey = empty($this->entityInfo['entity keys']['status']) ? 'status' : $this->entityInfo['entity keys']['status'];
  }

  /**
   * {@inheritdoc}
   */
  public function id() {
    return isset($this->{$this->nameKey}) ? $this->{$this->nameKey} : NULL;
  }
  
  /**
   * Returns the entity_info for the entity
   * 
   * @return array
   *  The entity_info as defined in hook_entity_info().
   */
  public function entityInfo() {
    return $this->entityInfo;
  }
  
  /**
   * {@inheritdoc}
   */
  public function entityType() {
    return $this->entityType;
  }

  /**
   * {@inheritdoc}
   */
  public function bundle() {
    return !empty($this->entityInfo['entity keys']['bundle']) ? $this->{$this->entityInfo['entity keys']['bundle']} : $this->entityType;
  }
  
  /**
   * A convenience method, it returns an entity_metadata_wrapper for the entity.
   */
  public function wrapper() {
    if (empty($this->wrapper)) {
      $this->wrapper = entity_metadata_wrapper($this->entityType, $this);
    }
    elseif ($this->wrapper->value() !== $this) {
      // Wrapper has been modified outside, so we'd better create a new one.
      $this->wrapper = entity_metadata_wrapper($this->entityType, $this);
    }
    return $this->wrapper;
  }

  /**
   * {@inheritdoc}
   */
  public function label() {
    return !empty($this->entityInfo['entity keys']['label']) ? $this->getTranslation($this->entityInfo['entity keys']['label']) : '';
  }
  
  /**
   * {@inheritdoc}
   */
  public function uri() {
    if (isset($this->entityInfo['uri callback'])) {
      $uri_callback = $this->entityInfo['uri callback'];
      if ($uri_callback == 'entity_uri' || $uri_callback == 'entity_class_uri') {
        // To avoid an infinite loop (since entity_uri() in Backdrop will call this method) 
        // we provide a defaultUri. Additionally, we check for the special callback
        // 'entity_class_uri' which was used in Drupal to indicate the default URI provided
        // by the entity class
        return $this->defaultUri();
      } else {
        // Call the callback with the entity
        return $uri_callback($this->entityType, $this);
      }
    } else {
      return $this->defaultUri();
    }
    return FALSE;
  }

  /**
   * Override this in order to implement a custom default URI within this class
   * or provide a custom callback in 'uri callback' in hook_entity_info().
   * 
   * This default URI is only provided when:
   *   - When 'entity_uri' or 'entity_class_uri' is provided as the entity's 'uri callback', OR
   *   - When 'uri callback' is not provided at all in hook_entity_info().
   */
  protected function defaultUri() {
    return array(
      'path' => 'default/' . $this->id(),
      'options' => array(),
    );
  }

  /**
   * Checks whether or not the entity has a specific status
   * 
   * @return boolean
   *    TRUE if the entity has a specific status
   */
  public function hasStatus($status) {
    if (!empty($this->entityInfo['exportable'])) {
      return isset($this->{$this->statusKey}) && ($this->{$this->statusKey} & $status) == $status;
    }
  }

  /**
   * {@inheritdoc}
   */
  public function isNew() {
    return !empty($this->is_new) || !$this->id();
  }

  /**
   * {@inheritdoc}
   */
  public function access($op, $account = NULL) {
    $rights = &backdrop_static("{$this->entityType}_access", array());

    // If no user object is supplied, the access check is for the current user.
    if (empty($account)) {
      $account = $GLOBALS['user'];
    }

    $entity_id = $this->id();
    $entity_type = $this->entityType();
    
    // If we've already checked access for this entity, user and op, return from
    // cache.
    if (isset($rights[$account->uid][$entity_type][$entity_id][$op])) {
      return $rights[$account->uid][$entity_type][$entity_id][$op];
    }
    
    $access_callback = $this->entityInfo['access callback'];
    $rights[$account->uid][$entity_type][$entity_id][$op] = $access_callback($op, $this, $account);
    return $rights[$account->uid][$entity_type][$entity_id][$op];
  }

  /**
   * {@inheritdoc}
   */
  public function save() {
    return entity_get_controller($this->entityType)->save($this);
  }

  /**
   * {@inheritdoc}
   */
  public function export($prefix = '') {
    return entity_get_controller($this->entityType)->export($this, $prefix);
  }

  /**
   * {@inheritdoc}
   */
  public function delete() {
    $id = $this->id();
    if (isset($id)) {
      entity_get_controller($this->entityType)->delete(array($id));
    }
  }

  /**
   * {@inheritdoc}
   */
  public function view($view_mode = 'full', $langcode = NULL, $page = NULL) {
    return entity_get_controller($this->entityType)->view(array($this), $view_mode, $langcode, $page);
  }


  /**
   * Returns a translated string if the module Internationalization exists
   */
  public function getTranslation($property, $langcode = NULL) {
    $all_info = entity_plus_get_all_property_info($this->entityType);
    // Assign by reference to avoid triggering notices if metadata is missing.
    $property_info = &$all_info[$property];

    if (!empty($property_info['translatable'])) {
      if (!empty($property_info['field'])) {
        return field_get_items($this->entityType, $this, $property, $langcode);
      }
      elseif (!empty($property_info['i18n string']) && module_exists('i18n')) {
       $name = $this->entityInfo['module'] . ':' . $this->entityType . ':' . $this->id() . ':' . $property;
       return entity_i18n_string($name, $this->$property, $langcode);
      }
    }
    return $this->$property;
  }

  /**
   * Indicates whether this is the default revision 
   */
  public function isDefaultRevision() {
    if (!empty($this->entityInfo['entity keys']['revision'])) {
      $key = !empty($this->entityInfo['entity keys']['default revision']) ? $this->entityInfo['entity keys']['default revision'] : 'default_revision';
      return !empty($this->$key);
    }
    return TRUE;
  }

  /** 
   * Indicates whether this entity is locked
   */
  public function isLocked() {
    return isset($this->status) && empty($this->is_new) && (($this->status & ENTITY_IN_CODE) || ($this->status & ENTITY_FIXED));
  }
}


class BasicEntityPlusController extends EntityPlusController {

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

  public function buildContent($entity, $view_mode = 'full', $langcode = NULL, $content = array()) {
    $wrapper = entity_metadata_wrapper('basic_entity_plus', $entity);
    
    $build = parent::buildContent($entity, $view_mode, $langcode, $content);

    // Theme the properties of the entity as fields
    // You also should implement hook_field_extra_fields() to allow 
    // the Field API UI to show those as fields  
    $field_defaults = array(
      '#theme' => 'field',
      '#access' => TRUE,
      '#label_display' => 'inline',
      '#view_mode' => $view_mode,
      '#language' => $langcode,
      '#field_type' => 'text',
      '#entity_type' => 'basic_entity_plus',
      '#bundle' => $entity->type,
      '#formatter' => 'text_default',
    );

    $build['title'] = array(
      '#title' => t('Title'),
      '#field_name' => 'field_fake_title',
      '#items' => array(array('value' => $wrapper->label())),
      0 => array('#markup' => $wrapper->label())
    ) + $field_defaults;

    $build['created'] = array(
      '#title' => t('Created'),
      '#field_name' => 'field_fake_created',
      '#items' => array(array('value' => format_date($wrapper->created->value(),'medium'))),
      0 => array('#markup' => format_date($wrapper->created->value(),'medium')),
    ) + $field_defaults;

    return $build;
  }
}

class BasicEntityPlusTypeController extends EntityPlusControllerExportable {
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
    // @see http://drupal.org/node/1399618
    state_set('menu_rebuild_needed', TRUE);
    return parent::save($entity, $transaction);
  }
}

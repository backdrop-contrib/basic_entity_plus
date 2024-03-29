<?php 

/**
 * Defines a basic entity plus class.
 * 
 * This class extends the core Entity class. It must define many of the undefined 
 * methods in the original class and interface
 */
class BasicEntityPlus extends Entity {

  protected $entityType; // Stores the entity type
  protected $entityInfo; // Stores the info provided in hook_entity_info() for this entity
  protected $idKey; // The key where entity id is stored.
  protected $wrapper; // Stores the entitiy_metadata_wrapper for this entity

  /**
   * Overrides Entity construct method to accept entityType.
   */
  public function __construct(array $values = array(), $entityType = NULL) {
    if (empty($entityType)) {
      throw new Exception('Cannot create an instance of Entity without a specified entity type.');
    }
    parent::__construct($values);

    $this->entityType = $entityType;
    $this->entityInfo = entity_get_info($this->entityType);
    $this->idKey = $this->entityInfo['entity keys']['id'];
  }

  /**
   * {@inheritdoc}
   * 
   * Returns the id of this entity.
   */
  public function id() {
    return $this->{$this->idKey};
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
    return $this->{$this->entityInfo['entity keys']['bundle']};
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
   * 
   * Returns the label of this entity.  
   */
  public function label() {
    return $this->getTranslation($this->entityInfo['entity keys']['label']);
  }
  
  /**
   * {@inheritdoc}
   * 
   * Returns the uri of this entity. This is hard-coded for this entity, but
   * an alternative is to define a uri callback in hook_entity_info().
   */
  public function uri() {
    return array(
      'path' => 'basic_entity_plus/' . $this->id(),
      'options' => array(),
    );
  }

  /**
   * Overrides Entity::createAccess().
   */
  public static function createAccess($bundle = NULL, $account = NULL) {
    global $user;
    $rights = &backdrop_static('basic_entity_plus_access', array());

    // If no user object is supplied, the access check is for the current user.
    if (empty($account)) {
      $account = $user;
    }

    // If we've already checked access for this user and op, return from static
    // cache.
    if (isset($rights[$account->uid][$bundle])) {
      return $rights[$account->uid][$bundle];
    }

    $rights[$account->uid][$bundle] = basic_entity_plus_access('create');
    return $rights[$account->uid][$bundle];
  }

  /**
   * {@inheritdoc}
   * 
   * This provides access check for view, create, update and delete.
   * Typically the checks can be done inside this method. Instead we defined
   * an access callback in hook_entity_info() for convenience. 
   */
  public function access($op, $account = NULL) {
    global $user;
    $rights = &backdrop_static('basic_entity_plus_access', array());

    if ($op == 'create') {
      return self::createAccess($this->bundle(), $account);
    }

    // If no user object is supplied, the access check is for the current user.
    if (empty($account)) {
      $account = $user;
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
   * 
   * Provided for convenience. Otherwise the controller will take care of this
   * when using entity_plus_save().
   */
  public function save() {
    return entity_get_controller($this->entityType)->save($this);
  }

  /**
   * {@inheritdoc}
   * 
   * Provided for convenience. Otherwise the controller will take care of this
   * when using entity_delete_multiple().
   */
  public function delete() {
    $id = $this->id();
    if (isset($id)) {
      entity_get_controller($this->entityType)->delete(array($id));
    }
  }

  /**
   * Returns a translated string if the module Internationalization exists.
   * 
   * This method was part of D7's Entity API class, but was removed in bAckdrop's
   * Entity class. It is mostly used to provide translations for entity properties. 
   */
  public function getTranslation($property, $langcode = NULL) {
    $all_info = entity_plus_get_all_property_info($this->entityType);
    // Assign by reference to avoid triggering notices if metadata is missing.
    $property_info = &$all_info[$property];

    if (!empty($property_info['translatable'])) {
      if (!empty($property_info['field'])) {
        return field_get_items($this->entityType, $this, $property, $langcode);
      }
      elseif (!empty($property_info['i18n string']) && module_exists('i18n_string')) {
        // This option requires the module i18n_string to be enabled. It also 
        // requires that the flag 'i18n string` be TRUE in hook_entity_property_info().
       $name = $this->entityInfo['module'] . ':' . $this->entityType . ':' . $this->id() . ':' . $property;
       return entity_plus_i18n_string($name, $this->$property, $langcode);
      }
    }
    return $this->$property;
  }

}

/** 
 * Defines a BasicEntityPlusType class.
 * 
 * This class is here to provide an object for basic_entity_plus_type entities.
 * These entities are configuration entities that define bundles for
 * basic_entity_plus entities.
 */
class BasicEntityPlusType extends Entity {

  protected $entityInfo; // stores the info provided in hook_entity_info() for this entity.
  protected $idKey; // Stores the key where the id is stored.
  protected $nameKey; // Stores the key where the name of the entity is stored.

  public function __construct(array $values = array(), $entityType = NULL) {
    if (empty($entityType)) {
      throw new Exception('Cannot create an instance of Entity without a specified entity type.');
    }
    parent::__construct($values);

    $this->entityType = $entityType;
    $this->entityInfo = entity_get_info($this->entityType);
    $this->idKey = $this->entityInfo['entity keys']['id'];
    $this->nameKey = $this->entityInfo['entity keys']['name'];
  }

  /**
   * {@inheritdoc}
   * 
   * Returns the id of this entity.
   */
  public function id() {
    return $this->{$this->idKey};
  }

  /** 
   * {@inheritdoc}
   * 
   * Since this is a configuration entity, there is no need to provide a uri.
   * Configuration entities don't have a 'page' for viewing them. 
   */
  function uri() { }

  function entityType() {
    return 'basic_entity_plus_type';
  }

  /** 
   * {@inheritdoc}
   * 
   * Return the human label for this entity.
   */
  function label() {
    return $this->{$this->entityInfo['entity keys']['label']};
  }
}

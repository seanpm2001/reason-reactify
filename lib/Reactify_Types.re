/*
 * Interface to bridge a React-style functional API
 * with a mutable back-end. Similiar in spirit
 * to the reconciler interface provided via 'react-conciler'.
 */

module type Reconciler = {
  /*
      Primitives is a variant type describing metadata needed
      to instantiate something in the mutable world.
   */
  type primitives;

  /*
      A node is a live instance representing a mutable object
   */
  type node;

  let appendChild: (node, node) => unit;

  let createInstance: primitives => node;

  let replaceChild: (node, node, node) => unit;

  let removeChild: (node, node) => unit;

  let updateInstance: (node, primitives, primitives) => unit;
};

module type React = {
  type primitives;
  type node;
  type hook('t);
  type state('a);

  type renderedElement =
    | RenderedPrimitive(node)
  and elementWithChildren = (childComponents, Effects.effects, Context.t)
  and render = unit => elementWithChildren
  and component =
    | Primitive(primitives, render)
    | Component(ComponentId.t, render)
    | Provider(render)
    | Empty(render)
  and primitiveComponent = (hook(unit), component)
  and childComponents = list(component);

  type t;

  /*
       Container API
   */
  type reconcileNotification = node => unit;
  let createContainer:
    (
      ~onBeginReconcile: reconcileNotification=?,
      ~onEndReconcile: reconcileNotification=?,
      node
    ) =>
    t;
  let updateContainer: (t, component) => unit;

  /*
       Component creation API
   */
  let primitiveComponent:
    (~children: list(component), primitives) => component;

  module type Component = {
    type hooks;
    type createElement;
    let createElement: createElement;
  };

  let createComponent:
    (('b, unit => component), component, 'a) =>
    (module Component with type createElement = 'd and type hooks = 'e);

  /*
       Component API
   */

  type providerConstructor('t) =
    (~children: childComponents, ~value: 't, unit) => component;
  type context('t);

  let getProvider: context('t) => providerConstructor('t);
  let createContext: 't => context('t);
  let useContext: context('t) => 't;

  let empty: component;

  let useEffect:
    (~condition: Effects.effectCondition=?, Effects.effectFunction) => unit;

  let useState:
    ('state, ('state, 'state => unit) => (hook('a), 'b)) =>
    (hook(('a, state('state))), 'b);

  let useReducer:
    (('state, 'action) => 'state, 'state) => ('state, 'action => unit);
};
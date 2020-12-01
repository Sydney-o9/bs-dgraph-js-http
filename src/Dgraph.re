[@bs.deriving abstract]
type operation = {
  [@bs.optional] schema: string,
  [@bs.optional] dropAttr: string,
  [@bs.optional] dropAll: bool,
};

[@bs.deriving abstract]
type mutation = {
  [@bs.optional] setJson: Js.Json.t,
  [@bs.optional] deleteJson: Js.Json.t,
  [@bs.optional] setNquads: string,
  [@bs.optional] deleteNquads: string,
  [@bs.optional] startTs: int,
  [@bs.optional] commitNow: bool,
  [@bs.optional] mutation: string,
  [@bs.optional] isJsonString: bool,
};

type request = {
  query: string,
};

type response = {
  data: Js.Json.t,
  hello: string
};

[@bs.deriving abstract]
type txnContext = {
  startTs: int,
  readOnly: bool,
  bestEffort: bool,
  [@bs.optional] aborted: bool,
  [@bs.optional] keys: array(string),
  [@bs.optional] preds: array(string),
};

[@bs.deriving abstract]
type latency = {
  [@bs.optional] parsingNs: int,
  [@bs.optional] processingNs: int,
  [@bs.optional] encodingNs: int,
};

type payload = {
  data: {.}
}

type extensions = {
  serverLatency: latency,
  txn: txnContext
};

type uids = { 
  k: string 
};

type assignedData = {
  // TODO: fix this as per below
  uids
};

// export interface AssignedData {
//     uids: {
//         [k: string]: string;
//     };
// }

type assigned = {
  data: assignedData,
  extensions
};

[@bs.deriving abstract]
type txnOptions = {
  [@bs.optional] readOnly: bool,
  [@bs.optional] bestEffort: bool
};

type txn;
type dgraphClient;
type dgraphClientStub;


module DgraphClientStub {
  type t = dgraphClientStub;

  [@bs.module "dgraph-js-http"] [@bs.new] external make: (string) => t = "DgraphClientStub"; 

  [@bs.send.pipe: t] external getHealth: bool => Js.Promise.t(response) = "getHealth";
  [@bs.send.pipe: t] external query: request => Js.Promise.t(response) = "query";
  [@bs.send.pipe: t] external setSlashApiKey: string => unit = "setSlashApiKey";
};

module DgraphClient {
  type t = dgraphClient;

  [@bs.module "dgraph-js-http"] [@bs.new] external make: (DgraphClientStub.t) => t = "DgraphClient"; 

  [@bs.send.pipe: t] external setQueryTimeout: int => unit = "setQueryTimeout";
  [@bs.send.pipe: t] external getQueryTimeout: unit => int = "getQueryTimeout";
  [@bs.send.pipe: t] external alter: operation => Js.Promise.t(payload) = "alter";
  [@bs.send.pipe: t] external setAlphaAuthToken: string => unit = "setAlphaAuthToken";
  [@bs.send.pipe: t] external setSlashApiKey: string => unit = "setSlashApiKey";
  [@bs.send.pipe: t] external login: (~userId: string=?, ~password: string=?, ~refreshToken: string=?) => Js.Promise.t(bool) = "login";
  [@bs.send.pipe: t] external logout: unit => unit = "logout";
  [@bs.send.pipe: t] external newTxn: (~options: txnOptions=?) => txn = "newTxn";
  [@bs.send.pipe: t] external setDebugMode: bool => unit = "setDebugMode";
};

module Txn {
  type t = txn;

  [@bs.module "dgraph-js-http"] [@bs.new] external make: (~dc: DgraphClient.t, ~options: txnOptions=?, unit) => t = "Txn"; 

  [@bs.send.pipe: t] external query: (~q: string) => Js.Promise.t(response) = "query";

  // ~vars is in fact Js.Dict.t(string) as per
  // https://github.com/dgraph-io/dgraph-js-http/blob/master/src/txn.ts#L62
  // Hence it it safer to pass a string in place of any
  [@bs.send.pipe: t] external queryWithVars: (~q: string, ~vars: Js.Dict.t(string)) => Js.Promise.t(response) = "queryWithVars";

  [@bs.send.pipe: t] external mutate: (~mu: mutation) => Js.Promise.t(assigned) = "mutate";
  [@bs.send.pipe: t] external commit: unit => Js.Promise.t(unit) = "commit";
  [@bs.send.pipe: t] external discard: unit => Js.Promise.t(unit) = "discard";
};
# DiffHSet

**DiffHSet** is a Redis module providing a wrapper command around `HSET` Redis command.
This wrapper publishes a rich notification on each successful invocation with the update data,
including the value before the update, the key being updated and the update itself.

## Building & Installation

**DiffHSet** requires CMake and a C++ compiler with C++20 support to be built.

The only external dependency is [nlohmann/json](https://json.nlohmann.me/).

The build sequence is standard for a CMake project, being the following:

```shell
mkdir cmake-build-release && cd cmake-build-release
cmake -GNinja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_LIBDIR=/var/lib/redis ..
cmake --build . -t install
```

Or, if you don't want the installation:

```shell
mkdir cmake-build-release && cd cmake-build-release
cmake -GNinja -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

This will build a self-contained shared library file `libdhset.so`,
which can be loaded into Redis through the config:

```
loadmodule /var/lib/redis/libdhset.so
```

Or dynamically, via command:

```redis
MODULE LOAD /var/lib/redis/libdhset.so
```

## Usage

By default, this module will publish notifications to `__modevent@{db}__:dhset` channel.
Messages are serialized with JSON by default and have the following structure:

```json
{
  "k": "hash key",
  "o": "'old' state as an object",
  "u": "operation arguments as an object"
}
```

For example, if we have a key `state` with the following content:

| Field      | Value           |
|------------|-----------------|
| `id`       | `101`           |
| `content`  | `Hello, World!` |
| `receiver` | `broadcast`     |

And we perform this operation:

```redis
DHSET state content "Hello, Redis!" receiver Redis
```

The following notification will be published on channel `__modevent@0__:dhset`:

```json
{
  "k": "state",
  "o": {
    "content": "Hello, World!",
    "id": "101",
    "receiver": "broadcast"
  },
  "u": {
    "content": "Hello, Redis!",
    "receiver": "Redis"
  }
}
```

Under the hood it uses `HSET` command to set the values, and if it fails for any reason,
no event will be generated and the original reply will be returned.

### Configuration

You can configure the module by setting these config options in `redis.conf`
or by executing `CONFIG SET`:

* `dhset.serialize-with [default=json]`

  This option sets the serializer for notification payload.
  There are 3 serializers available: `json`, `cbor` and `msgpack`.

* `dhset.notification-mode [default=channel]`

  This option is a flagset with two possible flags:
    * In `channel` mode it will publish notifications to `__modevent@{db}__:dhset`
      regardless of the key;
    * In `key` mode it will use a dedicated channel for each key like this: `__modevent@{db}__:dhset:{key}`

  Flags can be combined by specifying a whitespace-separated list:

  ```redis
    CONFIG SET dhset.notification-mode "key channel"
  ```
* `dhset.key-pattern [default=""]`

  A regular expression to select keys that should publish notifications.
  E.g. let the pattern be `model.*`. Executing the following statements:

  ```redis
    DHSET model:user id 12 name wunder
    DHSET session:12 expires 2023-12-07T12:33Z
  ```

  Will only give one notification for `model:user`, but not for `session:12`.
  If pattern is set to an empty string, filtering is disabled.
  > **NOTICE:** The pattern is matched against __the entire key__, not just a part of it that might match,
  > i.e. key `model:user` wouldn't match against `^model` or `:user$` patterns.
* `dhset.enable-key-caching [default=no]`

  You might want to enable this option if you're planning to do a lot
  of `DHSET`'s with key matching enabled (`dhset.key-pattern` is set),
  especially in cases where potential number of distinct keys is not very big.
  Instead of matching the same key against the same pattern every `DHSET` with that key,
  it stores the matching result in a cache. This cache is instance-local in a sense that
  it does not rely on Redis for caching. In fact, this is just a hashset of keys in the module's memory space.

  Enabling caching has no effect if key matching is disabled.

* `dhset.matched-key-cache-size [default=128]`

  Since the key cache is just an in-memory object we obviously need to limit its size.
  And there it is, a config option just for that.

* `dhset.shrink-matched-key-cache [default=flush]`

  Cache size limit enforcement strategy. If the cache have reached the size limit
  when a new key arrives, one of the following actions might be performed:
    * `flush` - the cache gets emptied completely
    * `halve` - half of cache entries get removed
    * `one-out` - exactly one cache entry gets removed
  
  Due to the nature of hash tables, the order in which cache entries are removed is not guaranteed.

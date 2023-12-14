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
    "content": {
      "bytes": [
        72,
        101,
        108,
        108,
        111,
        44,
        32,
        87,
        111,
        114,
        108,
        100,
        33
      ],
      "subtype": null
    },
    "id": {
      "bytes": [
        49,
        48,
        49
      ],
      "subtype": null
    },
    "receiver": {
      "bytes": [
        98,
        114,
        111,
        97,
        100,
        99,
        97,
        115,
        116
      ],
      "subtype": null
    }
  },
  "u": {
    "content": {
      "bytes": [
        72,
        101,
        108,
        108,
        111,
        44,
        32,
        82,
        101,
        100,
        105,
        115,
        33
      ],
      "subtype": null
    },
    "receiver": {
      "bytes": [
        82,
        101,
        100,
        105,
        115
      ],
      "subtype": null
    }
  }
}
```

Notice how the values are encoded. This is a special 'binary' type that used to serialize binary data safely.
However, it is pretty bloated in JSON and will not be natively converted back to byte array,
so this should be used only for debugging purposes. Consider using one of supported binary formats to use in production.

After processing the notification, execution will be redirected to
a ['delegate' command](#dhsetdelegate-to), and its reply
will be redirected back to caller.

### Startup options

#### Command hooking

This module provides only one startup option - the command that will be replaced with `DHSET` command.
E.g. if the following configuration is used to load DHSet:

```
loadmodule /var/lib/redis/libdhset.so hmset
```

It will intercept every Redis call and replace command name with `DHSET` if
original command name matches `hmset` (case-insensitive).

If the option is not provided, command hooking is disabled.

> **NOTE:** Command hooking blindly replaces **any** command that will be given as an argument, without any checks.
> Be aware to not accidentally replace e.g. plain `SET` command with it, it will just break it.

> **NOTE:** When using command hooking, ensure that the correct [delegate command](#dhsetdelegate-to) is configured.
> Hooking `HMSET` and using `HSET` delegate will basically replace `HMSET` with `HSET`, which may break some clients
> since these commands have different reply formats (and may as well behave differently on older Redis versions).

### Configuration options

These options can be configured in `redis.conf` or by using `CONFIG SET` command.

#### dhset.delegate-to

**Default value:** `hset`

This option allows to specify which delegate command will be used by `DHSET`.
`DHSET` is a wrapper command, it does not modify the database by itself (apart from sending notifications).
Instead, it passes the input to an actual command, being either `HSET` or `HMSET`.

This setting can be very useful in combination with [command hooking](#command-hooking),
as `HSET` and `HMSET` have different replies and may as well behave slightly differently.

#### dhset.serialize-with

**Default value:** `json`

This option sets the serializer for notification payload.
There are 3 serializers available: `json`, `cbor` and `msgpack`.

#### dhset.notification-mode

**Default value:** `channel`

This option is a flagset with two possible flags:

* In `channel` mode it will publish notifications to `__modevent@{db}__:dhset`
  regardless of the key;
* In `key` mode it will use a dedicated channel for each key like this: `__modevent@{db}__:dhset:{key}`

Flags can be combined by specifying a whitespace-separated list:

  ```redis
    CONFIG SET dhset.notification-mode "key channel"
  ```

#### dhset.key-pattern

**Default value:** ``

A regular expression to select keys that should publish notifications.
This can be useful if [command hooking](#command-hooking) is enabled
to only emit notifications when needed.
E.g. let the pattern be `model.*`. Executing the following statements:

  ```redis
    DHSET model:user id 12 name wunder
 DHSET session:12 expires 2023-12-07T12:33Z
  ```

Will only give one notification for `model:user`, but not for `session:12`.
If pattern is set to an empty string, filtering is disabled.

> **NOTICE:** The pattern is matched against __the entire key__, not just a part of it that might match,
> i.e. key `model:user` wouldn't match against `^model` or `:user$` patterns.

#### dhset.enable-key-caching

**Default value:** `no`

You might want to enable this option if you're planning to do a lot
of `DHSET`'s with key matching enabled (`dhset.key-pattern` is set),
especially in cases where potential number of distinct keys is not very big.
Instead of matching the same key against the same pattern every `DHSET` with that key,
it stores the matching result in a cache. This cache is instance-local in a sense that
it does not rely on Redis for caching. In fact, this is just a hashset of keys in the module's memory space.

Enabling caching has no effect if key matching is disabled.

#### dhset.matched-key-cache-size

**Default value:** `1000`

Since the key cache is just an in-memory object we obviously need to limit its size.
And there it is, a config option just for that.

#### dhset.shrink-matched-key-cache

**Default value:** `flush`

Cache size limit enforcement strategy. If the cache have reached the size limit
when a new key arrives, one of the following actions might be performed:

* `flush` - the cache gets emptied completely
* `halve` - half of cache entries get removed
* `one-out` - exactly one cache entry gets removed

Due to the nature of hash tables, the order in which cache entries are removed is not guaranteed.

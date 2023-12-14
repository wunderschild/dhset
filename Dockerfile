FROM alpine:latest AS build

RUN mkdir -p /tmp/build

RUN apk add --no-cache \
    cmake clang lld ninja compiler-rt musl-dev libc++ libc++-dev

COPY . /tmp/build/

RUN mkdir -p /tmp/build/build-release

WORKDIR /tmp/build/build-release

RUN cmake .. -GNinja \
    -DCMAKE_BUILD_TYPE=Release  \
    -DCMAKE_CXX_FLAGS="-fuse-ld=lld --rtlib=compiler-rt" \
    -DDHSET_GNU_STATIC=ON \
    -DOUTPUT_LIBC_SUFFIX=musl-static

RUN cmake --build .

FROM scratch AS export

COPY --from=build /tmp/build/build-release/libdhset*.so .
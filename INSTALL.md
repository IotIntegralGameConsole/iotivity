# Installing IoTivity

Once IoTivity has been built, it can be installed similarly to **autotools** or
**cmake**:

```
# scons [FLAGS] install
```


## Controlling the installation output

- Installation prefix:

  Default installation prefix is `/usr/local`; however it can be changed by
  `--prefix=PREFIX/PATH`.

  E.g.:

  ```
  # scons [FLAGS] --prefix=/usr install
  ```

  or

  ```
  # PREFIX=/usr scons [FLAGS] install
  ```

- Destination directory

  By default, destination directory is set to `/`, so IoTivity is installed to
  root directory.

  This can be easily adjusted, eg. for packaging purposes.

  ```
  # scons [FLAGS] --install-sandbox=/destination/directory install
  ```

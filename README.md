# WHAIS

htttp://www.whais.net
(to be available in the next days)

Whais proposes a new type of database server. Traditional SQL based system are very optimized at storing and retrieve chunks of data. However, these are not designed to process the information they store and this situation leads to certain inconsistency of the stored information and longer times to retrieve it. 

This new approach has at its core a new language designed for data manipulation. Differently from SQL, it's not designed to retrieve the information using queries, but to compute it procedurally based on the stored information. The result will be actually based on the analysis of the stored data. 

There are a couple of benefits with this approach. It allows a true validation of information before is actually stored due to ability to analyze the data in a procedural way. It lowers the times needed to retrieve relevant information as the gathering processing will be as close to the its location and possibility to use custom algorithms for the processing.

## Instalation
The work at this project is still very much in progress. There a no installers available yet, so the best way to use this is clone this repository and build it by yourself.  

The main development was done using the GNU/Linux environment in mind however this should build and work with Windows too. At the moment both versions require some development skill in order to use it: 

```

git clone git@github.com:iulianpopa1981/whais.git
make -j 8 all ARCH=linux_gcc_x86_64 FLAVOR=release
```


In case the build fails, you may need some development packages installed on your host. For instance, the WHAS compiler requires bison utility in order to generate the code parser.


## Contributing

Everyone is welcome to contribute in any way to improve this program. Even if you have just an idea how to this, please do share it here.  Otherwise:  

1. Fork it!  

2. Recall to have fun while you do all next steps. Chances are you are going to change the world a little bit.

3. Create your feature branch: `git checkout -b my-new-feature`  

4. Commit your changes: `git commit -am 'Add some feature'`  

5. Push to the branch: `git push origin my-new-feature` 

6. Submit a pull request :D 

## History

19th August 2018 -  

First version publicly available 


## License

WHAIS - A program used to manipulate and store information.

Copyright (C) 2018 Iulian Popa (popaiulian@gmail.com)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.



---

Login
> -> LSIN [trid](trid.md) [userid](userid.md) [password](password.md) 0
> <- LSIN [trid](trid.md) [code](error.md) 0

Capability
> -> CAPA [trid](trid.md) 0
> <- CAPA [trid](trid.md) [code](error.md) [string](capability.md) 0

Status
> -> STAT [trid](trid.md) 0
> <- STAT [trid](trid.md) [code](error.md) [string](status.md) 0

Logout
> -> LOUT [trid](trid.md) 0
> <- LOUT [trid](trid.md) [code](error.md) 0


---


List Table
> -> LTAB [trid](trid.md) 0
> <- LTAB [trid](trid.md) [code](error.md) [size](body.md)
> > table name
> > table name
> > ...

Show Table

> -> STAB [trid](trid.md) [name](table.md) 0
> <- STAB [trid](trid.md) [code](error.md) [size](body.md)
> > [name](key.md) [type](type.md) [size](size.md)
> > [name](column.md) [type](type.md) [size](size.md)
> > ...

Create Table

> -> CTAB [trid](trid.md) [name](table.md) [size](body.md)
> > [name](column.md) [type](type.md) [size](size.md)
> > [name](column.md) [type](type.md) [size](size.md)
> > ...

> <- CTAB [trid](trid.md) [code](error.md) 0

Delete Table
> -> DTAB [trid](trid.md) [name](table.md) 0
> <- DTAB [trid](trid.md) [code](error.md) 0

Use Table
> -> UTAB [trid](trid.md) [name](table.md) 0
> <- UTAB [trid](trid.md) [code](error.md) 0


---


Insert Row
> -> IROW [trid](trid.md) [size](body.md)
> > [name](column.md) [value](key.md)
> > [name](column.md) [value](value.md)
> > ...

> <- IROW [trid](trid.md) [code](error.md) 0

Select Row
> -> SROW [trid](trid.md) [key](key.md) [name](column.md) 0
> <- SROW [trid](trid.md) [code](error.md) [size](body.md)
> > value

Update Row

> -> UROW [trid](trid.md) [key](key.md) [name](column.md) [size](body.md)
> > value

> <- UROW [trid](trid.md) [code](error.md) 0

Delete Row
> -> DROW [trid](trid.md) [key](key.md) 0
> <- DROW [trid](trid.md) [code](error.md) 0


---

import React from 'react'
import { createStyles, makeStyles, Theme, useTheme } from '@material-ui/core/styles'
import {
    Grid,
    Table,
    TableBody,
    TableCell,
    TableContainer,
    TableHead,
    TablePagination,
    TableRow,
    TableSortLabel,
    Typography,
    withStyles
} from '@material-ui/core'

/**
 * LogsPage
 * 
 * TODO: Abstract this into components!
 */

interface Data {
    type: string,
    alarm: string,
    time: string, // Note: Make this a date object?
    details: number // Note: Make this an ID to view more details? 
}

function createData(
    type: string,
    alarm: string,
    time: string,
    details: number
): Data {
    return { type, alarm, time, details }
}

const rows = [
    createData('Operator', 'Peep above upper limit', '12:30 p.m. 07/14/2020', 1),
    createData('System', 'Peep above upper limit', '12:10 p.m. 07/14/2020', 1),
    createData('Patient', 'Peep above upper limit', '12:00 p.m. 07/14/2020', 1),
    createData('System', 'Peep above upper limit', '1:30 p.m. 07/14/2020', 1),
    createData('Operator', 'Peep above upper limit', '9:30 p.m. 07/14/2020', 1),
    createData('System', 'Peep above upper limit', '10:30 p.m. 07/14/2020', 1),
    createData('Patient', 'Peep above upper limit', '12:30 p.m. 07/14/2020', 1),
    createData('Patient', 'Peep above upper limit', '12:30 p.m. 07/14/2020', 1),
    createData('Patient', 'Peep above upper limit', '12:30 p.m. 07/14/2020', 1),
    createData('Patient', 'Peep above upper limit', '12:30 p.m. 07/14/2020', 1),
]

function descendingComparator<T>(a: T, b: T, orderBy: keyof T) {
    if (b[orderBy] < a[orderBy]) {
        return -1
    }
    if (b[orderBy] > a[orderBy]) {
        return 1
    }
    return 0
}

type Order = 'asc' | 'desc'

function getComparator<Key extends keyof any>(
    order: Order,
    orderBy: Key,
): (a: { [key in Key]: number | string }, b: { [key in Key]: number | string }) => number {
    return order === 'desc'
        ? (a, b) => descendingComparator(a, b, orderBy)
        : (a, b) => -descendingComparator(a, b, orderBy)
}

function stableSort<T>(array: T[], comparator: (a: T, b: T) => number) {
    const stabilizedThis = array.map((el, index) => [el, index] as [T, number])
    stabilizedThis.sort((a, b) => {
        const order = comparator(a[0], b[0])
        if (order !== 0) return order
        return a[1] - b[1]
    })
    return stabilizedThis.map((el) => el[0])
}

interface HeadCell {
    disablePadding: boolean,
    id: keyof Data,
    label: string,
    numeric: boolean
}

const headCells: HeadCell[] = [
    { id: 'type', numeric: false, disablePadding: true, label: 'Type' },
    { id: 'alarm', numeric: true, disablePadding: false, label: 'Alarm' },
    { id: 'time', numeric: true, disablePadding: false, label: 'Time/Date' },
    { id: 'details', numeric: true, disablePadding: false, label: 'Details' },
]

interface EnhancedTableProps {
    classes: ReturnType<typeof useStyles>
    numSelected: number
    onRequestSort: (event: React.MouseEvent<unknown>, property: keyof Data) => void
    onSelectAllClick: (event: React.ChangeEvent<HTMLInputElement>) => void
    order: Order
    orderBy: string
    rowCount: number
}

const StyledTableRow = withStyles((theme: Theme) =>
    createStyles({
        root: {
            '&:nth-of-type(odd)': {
                backgroundColor: theme.palette.primary.dark,
            },
        },
    }),
)(TableRow)

const StyledTableCell = withStyles((theme: Theme) =>
    createStyles({
        head: {
            backgroundColor: theme.palette.background.paper,
            borderBottom: '1px solid' + theme.palette.text.primary
        },
        body: {
            fontSize: 14,
        },
    }),
)(TableCell)

function EnhancedTableHead(props: EnhancedTableProps) {
    const { classes, onSelectAllClick, order, orderBy, numSelected, rowCount, onRequestSort } = props
    const createSortHandler = (property: keyof Data) => (event: React.MouseEvent<unknown>) => {
        onRequestSort(event, property)
    }

    return (
        <TableHead >
            <TableRow>
                {headCells.map((headCell) => (
                    <StyledTableCell
                        key={headCell.id}
                        align='left'
                        padding='default'
                        sortDirection={orderBy === headCell.id ? order : false}
                    >
                        <TableSortLabel
                            active={orderBy === headCell.id}
                            direction={orderBy === headCell.id ? order : 'asc'}
                            onClick={createSortHandler(headCell.id)}
                        >
                            {headCell.label}
                            {orderBy === headCell.id ? (
                                <span className={classes.visuallyHidden}>
                                    {order === 'desc' ? 'sorted descending' : 'sorted ascending'}
                                </span>
                            ) : null}
                        </TableSortLabel>
                    </StyledTableCell>
                ))}
            </TableRow>
        </TableHead>
    )
}

const useStyles = makeStyles((theme: Theme) =>
    createStyles({
        root: {
            width: '100%',
            // border: '1px solid yellow',
        },
        tableContainer: {
            width: '100%',
            border: '1px solid black',
            borderRadius: theme.panel.borderRadius
        },
        table: {
            minWidth: 750,
            padding: '3px solid black',
            backgroundColor: theme.palette.background.paper
        },
        visuallyHidden: {
            border: 0,
            clip: 'rect(0 0 0 0)',
            height: 1,
            margin: -1,
            overflow: 'hidden',
            padding: 0,
            position: 'absolute',
            top: 20,
            width: 1,
        },
        typeWrapper: {
            width:'50%',
            textAlign: 'center',
            borderRadius: 3,
            padding:4,
        }
    }),
)

/**
 * LogsPage
 */
export const LogsPage = () => {
    const classes = useStyles()
    const [order, setOrder] = React.useState<Order>('asc')
    const [orderBy, setOrderBy] = React.useState<keyof Data>('type')
    const [selected, setSelected] = React.useState<string[]>([])
    const [page, setPage] = React.useState(0)
    const [rowsPerPage, setRowsPerPage] = React.useState(8)

    const handleRequestSort = (event: React.MouseEvent<unknown>, property: keyof Data) => {
        const isAsc = orderBy === property && order === 'asc'
        setOrder(isAsc ? 'desc' : 'asc')
        setOrderBy(property)
    }

    const handleSelectAllClick = (event: React.ChangeEvent<HTMLInputElement>) => {
        if (event.target.checked) {
            const newSelecteds = rows.map((n) => n.type)
            setSelected(newSelecteds)
            return
        }
        setSelected([])
    }

    const handleClick = (event: React.MouseEvent<unknown>, name: string) => {
        const selectedIndex = selected.indexOf(name)
        let newSelected: string[] = []

        if (selectedIndex === -1) {
            newSelected = newSelected.concat(selected, name)
        } else if (selectedIndex === 0) {
            newSelected = newSelected.concat(selected.slice(1))
        } else if (selectedIndex === selected.length - 1) {
            newSelected = newSelected.concat(selected.slice(0, -1))
        } else if (selectedIndex > 0) {
            newSelected = newSelected.concat(
                selected.slice(0, selectedIndex),
                selected.slice(selectedIndex + 1),
            )
        }

        setSelected(newSelected)
    }

    const handleChangePage = (event: unknown, newPage: number) => {
        setPage(newPage)
    }

    const handleChangeRowsPerPage = (event: React.ChangeEvent<HTMLInputElement>) => {
        setRowsPerPage(parseInt(event.target.value, 10))
        setPage(0)
    }

    const isSelected = (name: string) => selected.indexOf(name) !== -1

    const emptyRows = rowsPerPage - Math.min(rowsPerPage, rows.length - page * rowsPerPage)

    const theme = useTheme()
    function typeColor (type:string) {
        if (type==='Operator') return {backgroundColor:theme.palette.primary.main}
        else if (type==='Patient') return {backgroundColor:'#92D25B', color:'black'}
        else if (type==='System') return {backgroundColor:'#E68619'}
        return {backgroundColor:'#E68619'}
    }

    return (
        <Grid
            container
            direction="column"
            justify="space-between"
            alignItems="stretch"
            className={classes.root}
        >
            <Grid item>
                <Typography variant='h3'>Events Log</Typography>
            </Grid>
            <Grid item >
                <TableContainer className={classes.tableContainer}>
                    <Table
                        className={classes.table}
                        aria-labelledby="tableTitle"
                        size={'medium'}
                        aria-label="enhanced table"
                    >
                        <EnhancedTableHead
                            classes={classes}
                            numSelected={selected.length}
                            order={order}
                            orderBy={orderBy}
                            onSelectAllClick={handleSelectAllClick}
                            onRequestSort={handleRequestSort}
                            rowCount={rows.length}
                        />
                        <TableBody>
                            {stableSort(rows, getComparator(order, orderBy))
                                .slice(page * rowsPerPage, page * rowsPerPage + rowsPerPage)
                                .map((row, index) => {
                                    const isItemSelected = isSelected(row.type)
                                    const labelId = `enhanced-table-checkbox-${index}`

                                    return (
                                        <StyledTableRow
                                            hover
                                            onClick={(event) => handleClick(event, row.type)}
                                            tabIndex={-1}
                                            key={row.details}
                                        >
                                            <TableCell align='left' component="th" id={labelId} scope="row" >
                                                <Grid
                                            className={classes.typeWrapper}
                                            style={typeColor(row.type)}>
                                                    {row.type}
                                                </Grid>
                                            </TableCell>
                                            <TableCell align='left' component="th" id={labelId} scope="row" >
                                                {row.alarm}
                                            </TableCell>
                                            <TableCell align="left">{row.time}</TableCell>
                                            <TableCell align="left">{row.details}</TableCell>
                                        </StyledTableRow>
                                    )
                                })}
                            {emptyRows > 0 && (
                                <TableRow style={{ height: 53 * emptyRows }}>
                                    <TableCell colSpan={6} />
                                </TableRow>
                            )}
                        </TableBody>
                    </Table>
                </TableContainer>
            </Grid>
            <Grid item>
                <TablePagination
                    rowsPerPageOptions={[8]}
                    component="div"
                    count={rows.length}
                    rowsPerPage={rowsPerPage}
                    page={page}
                    onChangePage={handleChangePage}
                    onChangeRowsPerPage={handleChangeRowsPerPage}
                    style={{
                        width: '100%'
                    }}
                />
            </Grid>
        </Grid>
    )
}

export default LogsPage
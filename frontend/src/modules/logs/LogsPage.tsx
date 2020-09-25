import React from 'react';
import { createStyles, makeStyles, Theme, useTheme } from '@material-ui/core/styles';
import { Grid, TableCell, TableRow, Typography } from '@material-ui/core';
import SimpleTable, {
  stableSort,
  getComparator,
  StyledTableRow,
  Order,
  HeadCell,
} from '../controllers/SimpleTable';
import ModalPopup from '../controllers/ModalPopup';
import EventlogDetails from './container/EventlogDetails';
import { DECIMAL_RADIX } from '../app/AppConstants';

/**
 * LogsPage
 *
 * TODO: Abstract this into components!
 */

interface Data {
  type: string;
  alarm: string;
  time: number; // Note: Make this a date object?
  details: string; // Note: Make this an ID to view more details?,
  id: number;
}

function createData(type: string, alarm: string, time: number, details: string, id: number): Data {
  return { type, alarm, time, details, id };
}

const rows = [
  createData(
    'Operator',
    'Peep above upper limit',
    parseInt((new Date('2020-09-09 10:11:00').getTime() / 1000).toFixed(0), DECIMAL_RADIX),
    'View Details',
    1,
  ),
  createData(
    'System',
    'Peep above upper limit',
    parseInt((new Date('2020-09-09 10:12:00').getTime() / 1000).toFixed(0), DECIMAL_RADIX),
    'View Details',
    2,
  ),
  createData(
    'Patient',
    'Peep above upper limit',
    parseInt((new Date('2020-09-09 10:13:00').getTime() / 1000).toFixed(0), DECIMAL_RADIX),
    'View Details',
    3,
  ),
  createData(
    'System',
    'Peep above upper limit',
    parseInt((new Date('2020-09-09 09:11:00').getTime() / 1000).toFixed(0), DECIMAL_RADIX),
    'View Details',
    4,
  ),
  createData(
    'Operator',
    'Peep above upper limit',
    parseInt((new Date('2020-09-08 10:10:00').getTime() / 1000).toFixed(0), DECIMAL_RADIX),
    'View Details',
    5,
  ),
  createData(
    'System',
    'Peep above upper limit',
    parseInt((new Date('2020-09-08 10:11:00').getTime() / 1000).toFixed(0), DECIMAL_RADIX),
    'View Details',
    6,
  ),
  createData(
    'Patient',
    'Peep above upper limit',
    parseInt((new Date('2020-09-08 10:12:00').getTime() / 1000).toFixed(0), DECIMAL_RADIX),
    'View Details',
    7,
  ),
  createData(
    'Patient',
    'Peep above upper limit',
    parseInt((new Date('2020-09-07 10:10:00').getTime() / 1000).toFixed(0), DECIMAL_RADIX),
    'View Details',
    8,
  ),
  createData(
    'Patient',
    'Peep above upper limit',
    parseInt((new Date('2020-09-07 10:11:00').getTime() / 1000).toFixed(0), DECIMAL_RADIX),
    'View Details',
    9,
  ),
  createData(
    'Patient',
    'Peep above upper limit',
    parseInt((new Date('2020-09-07 10:12:00').getTime() / 1000).toFixed(0), DECIMAL_RADIX),
    'View Details',
    10,
  ),
];

const headCells: HeadCell[] = [
  { id: 'type', numeric: false, disablePadding: true, label: 'Type' },
  { id: 'alarm', numeric: true, disablePadding: false, label: 'Alarm' },
  { id: 'time', numeric: true, disablePadding: false, label: 'Time/Date' },
  { id: 'details', numeric: false, disablePadding: false, label: 'Details' },
];

const useStyles = makeStyles((theme: Theme) =>
  createStyles({
    root: {
      width: '100%',
      // border: '1px solid yellow',
    },
    typeWrapper: {
      width: '50%',
      textAlign: 'center',
      borderRadius: 3,
      padding: 4,
    },
    typeWrapper2: {
      fontSize: 16,
      textAlign: 'center',
      borderRadius: 3,
      padding: 4,
    },
  }),
);

/**
 * LogsPage
 */
export const LogsPage = (): JSX.Element => {
  const classes = useStyles();
  const theme = useTheme();
  const [order, setOrder] = React.useState<Order>('desc');
  const [orderBy, setOrderBy] = React.useState<keyof Data>('time');
  const [selected, setSelected] = React.useState<string[]>([]);
  const [page, setPage] = React.useState(0);
  const [rowsPerPage, setRowsPerPage] = React.useState(8);
  const [open, setOpen] = React.useState(false);
  const [currentRow, setCurrentRow] = React.useState<Data>();
  const isSelected = (name: string) => selected.indexOf(name) !== -1;
  const emptyRows = rowsPerPage - Math.min(rowsPerPage, rows.length - page * rowsPerPage);

  const handleClose = () => {
    setOpen(false);
  };

  const handleConfirm = () => {
    handleClose();
  };

  const typeColor = (type: string | undefined) => {
    if (type === 'Operator') return { backgroundColor: theme.palette.primary.main };
    if (type === 'Patient') return { backgroundColor: '#92D25B', color: 'black' };
    if (type === 'System') return { backgroundColor: '#E68619' };
    return { backgroundColor: '#E68619' };
  };

  const handleClick = (event: React.MouseEvent<unknown>, name: string) => {
    const selectedIndex = selected.indexOf(name);
    let newSelected: string[] = [];

    if (selectedIndex === -1) {
      newSelected = newSelected.concat(selected, name);
    } else if (selectedIndex === 0) {
      newSelected = newSelected.concat(selected.slice(1));
    } else if (selectedIndex === selected.length - 1) {
      newSelected = newSelected.concat(selected.slice(0, -1));
    } else if (selectedIndex > 0) {
      newSelected = newSelected.concat(
        selected.slice(0, selectedIndex),
        selected.slice(selectedIndex + 1),
      );
    }
    setSelected(newSelected);
  };

  return (
    <Grid
      container
      direction="column"
      justify="space-between"
      alignItems="stretch"
      className={classes.root}
    >
      <Grid item>
        <Typography variant="h3">Events Log</Typography>
      </Grid>

      <SimpleTable
        order={order}
        setOrder={setOrder}
        orderBy={orderBy}
        setOrderBy={setOrderBy}
        selected={selected}
        setSelected={setSelected}
        page={page}
        setPage={setPage}
        rowsPerPage={rowsPerPage}
        setRowsPerPage={setRowsPerPage}
        headCells={headCells}
        rows={rows}
      >
        {stableSort(rows, getComparator(order, orderBy))
          .slice(page * rowsPerPage, page * rowsPerPage + rowsPerPage)
          .map((row, index) => {
            const isItemSelected = isSelected(row.type);
            const labelId = `enhanced-table-checkbox-${index}`;

            return (
              <StyledTableRow
                hover
                onClick={(event: React.MouseEvent<unknown>) => handleClick(event, row.type)}
                tabIndex={-1}
                key={row.details}
              >
                <TableCell align="left" component="th" id={labelId} scope="row">
                  <Grid className={classes.typeWrapper} style={typeColor(row.type)}>
                    {row.type}
                  </Grid>
                </TableCell>
                <TableCell align="left" component="th" id={labelId} scope="row">
                  {row.alarm}
                </TableCell>
                <TableCell align="left">
                  {`
                                        ${new Date(row.time * 1000).toLocaleTimeString([], {
                                          hour: '2-digit',
                                          minute: '2-digit',
                                        })}
                                        ${new Date(row.time * 1000).toLocaleDateString([], {
                                          month: '2-digit',
                                          day: '2-digit',
                                          year: 'numeric',
                                        })}
                                    `}
                </TableCell>
                <TableCell
                  align="left"
                  onClick={() => {
                    setCurrentRow(row);
                    setOpen(true);
                  }}
                >
                  <Typography style={{ cursor: 'pointer' }} variant="inherit">
                    {row.details}
                  </Typography>
                </TableCell>
              </StyledTableRow>
            );
          })}
        {emptyRows > 0 && (
          <TableRow style={{ height: 53 * emptyRows }}>
            <TableCell colSpan={6} />
          </TableRow>
        )}
      </SimpleTable>
      <ModalPopup
        showCloseIcon={true}
        label={
          <Grid container direction="row" justify="space-around" alignItems="center">
            <Grid xs={2} className={classes.typeWrapper2} style={typeColor(currentRow?.type)}>
              {currentRow?.type}
            </Grid>
            <Grid xs={9}>
              <Typography variant="h4">{currentRow?.alarm}</Typography>
            </Grid>
          </Grid>
        }
        open={open}
        onClose={handleClose}
        onConfirm={handleConfirm}
      >
        <Grid item>
          <EventlogDetails />
        </Grid>
      </ModalPopup>
    </Grid>
  );
};

export default LogsPage;

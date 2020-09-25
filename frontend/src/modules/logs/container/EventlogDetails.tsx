import React from 'react';
import { createStyles, makeStyles, Theme, useTheme } from '@material-ui/core/styles';
import { Grid, TableCell, TableRow } from '@material-ui/core';
import SimpleTable, {
  stableSort,
  Order,
  getComparator,
  StyledTableRow,
  HeadCell,
} from '../../controllers/SimpleTable';

/**
 * Event log details
 *
 */

interface Data {
  value: string;
  measurement: string;
  id: number;
}

function createData(value: string, measurement: string, id: number): Data {
  return { value, measurement, id };
}

const rows = [
  createData('Respiratory Rate', '18 b/min', 1),
  createData('Respiratory Rate', '18 b/min', 2),
  createData('Respiratory Rate', '18 b/min', 3),
  createData('Respiratory Rate', '18 b/min', 4),
  createData('Respiratory Rate', '18 b/min', 5),
  createData('Respiratory Rate', '18 b/min', 6),
  createData('Respiratory Rate', '18 b/min', 7),
  createData('Respiratory Rate', '18 b/min', 8),
  createData('Respiratory Rate', '18 b/min', 9),
  createData('Respiratory Rate', '18 b/min', 10),
  createData('Respiratory Rate', '18 b/min', 11),
  createData('Respiratory Rate', '18 b/min', 12),
  createData('Respiratory Rate', '18 b/min', 13),
  createData('Respiratory Rate', '18 b/min', 14),
  createData('Respiratory Rate', '18 b/min', 15),
];

const headCells: HeadCell[] = [
  { id: 'value', numeric: false, disablePadding: true, label: 'Value' },
  { id: 'measurement', numeric: false, disablePadding: false, label: 'Measurement' },
];

const useStyles = makeStyles((theme: Theme) =>
  createStyles({
    typeWrapper: {
      width: '50%',
      textAlign: 'center',
      borderRadius: 3,
      padding: 4,
    },
  }),
);

/**
 * Event Logs detail page
 */
export const EventlogDetails = (): JSX.Element => {
  const classes = useStyles();
  const theme = useTheme();
  const [order, setOrder] = React.useState<Order>('asc');
  const [orderBy, setOrderBy] = React.useState<keyof Data>('value');
  const [selected, setSelected] = React.useState<string[]>([]);
  const [page, setPage] = React.useState(0);
  const [rowsPerPage, setRowsPerPage] = React.useState(5);
  const [open, setOpen] = React.useState(false);
  const isSelected = (name: string) => selected.indexOf(name) !== -1;
  const emptyRows = rowsPerPage - Math.min(rowsPerPage, rows.length - page * rowsPerPage);

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

  function makeRed(id: number) {
    if (id === 3) return { color: theme.palette.error.main };
    return {};
  }

  return (
    <Grid>
      <SimpleTable
        order={order}
        setOrder={setOrder}
        selected={selected}
        setSelected={setSelected}
        page={page}
        setPage={setPage}
        rowsPerPage={rowsPerPage}
        setRowsPerPage={setRowsPerPage}
        headCells={headCells}
        rows={rows}
        footer="12:30 p.m. 07/14/2020"
      >
        {stableSort(rows, getComparator(order, orderBy))
          .slice(page * rowsPerPage, page * rowsPerPage + rowsPerPage)
          .map((row, index) => {
            const isItemSelected = isSelected(row.value);
            const labelId = `enhanced-table-checkbox-${index}`;

            return (
              <StyledTableRow
                hover
                onClick={(event: React.MouseEvent<unknown>) => handleClick(event, row.value)}
                tabIndex={-1}
                key={row.id}
              >
                <TableCell align="left" style={makeRed(row.id)}>
                  {row.value}
                </TableCell>
                <TableCell align="left" style={makeRed(row.id)}>
                  {row.measurement}
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
    </Grid>
  );
};

export default EventlogDetails;

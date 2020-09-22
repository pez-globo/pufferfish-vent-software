/* eslint-disable @typescript-eslint/no-explicit-any */
import React, { PropsWithChildren, ReactNode } from 'react';
import { createStyles, makeStyles, Theme, useTheme } from '@material-ui/core/styles';
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
  withStyles,
} from '@material-ui/core';
import { DECIMAL_RADIX } from '../app/AppConstants';

/**
 * Simple table reusable component
 *
 */

function descendingComparator<T>(a: T, b: T, orderBy: keyof T) {
  if (b[orderBy] < a[orderBy]) {
    return -1;
  }
  if (b[orderBy] > a[orderBy]) {
    return 1;
  }
  return 0;
}

export type Order = 'asc' | 'desc';

export function getComparator<Key extends keyof number | string>(
  order: Order,
  orderBy: Key,
): (a: { [key in Key]: number | string }, b: { [key in Key]: number | string }) => number {
  return order === 'desc'
    ? (a, b) => descendingComparator(a, b, orderBy)
    : (a, b) => -descendingComparator(a, b, orderBy);
}

export function stableSort<T>(array: T[], comparator: (a: T, b: T) => number): T[] {
  const stabilizedThis = array.map((el, index) => [el, index] as [T, number]);
  stabilizedThis.sort((a, b) => {
    const order = comparator(a[0], b[0]);
    if (order !== 0) return order;
    return a[1] - b[1];
  });
  return stabilizedThis.map((el) => el[0]);
}

const useStyles = makeStyles((theme: Theme) =>
  createStyles({
    tableContainer: {
      width: '100%',
      border: `2px dashed ${theme.palette.background.default}`,
      borderRadius: theme.panel.borderRadius,
    },
    table: {
      minWidth: 500,
      padding: '3px solid black',
      backgroundColor: theme.palette.background.paper,
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
      width: '50%',
      textAlign: 'center',
      borderRadius: 3,
      padding: 4,
    },
    footer: {
      fontFamily: 'sans-serif',
      fontSize: 16,
      marginTop: 15,
      fontWeight: 100,
    },
  }),
);

export interface HeadCell {
  disablePadding: boolean;
  id: string | number;
  label: string;
  numeric: boolean;
}

export interface EnhancedTableProps {
  classes: ReturnType<typeof useStyles>;
  numSelected: number;
  onRequestSort: (event: React.MouseEvent<unknown>, property: string | number) => void;
  onSelectAllClick: (event: React.ChangeEvent<HTMLInputElement>) => void;
  order: Order;
  orderBy?: string;
  rowCount: number;
  headCells: HeadCell[];
}

export const StyledTableRow = withStyles((theme: Theme) =>
  createStyles({
    root: {
      '&:nth-of-type(odd)': {
        backgroundColor: theme.palette.primary.dark,
      },
    },
  }),
)(TableRow);

const StyledTableCell = withStyles((theme: Theme) =>
  createStyles({
    head: {
      backgroundColor: theme.palette.background.paper,
      borderBottom: `1px solid${theme.palette.text.primary}`,
    },
    body: {
      fontSize: 14,
    },
  }),
)(TableCell);

function EnhancedTableHead(props: EnhancedTableProps) {
  const {
    classes,
    onSelectAllClick,
    order,
    orderBy,
    numSelected,
    rowCount,
    onRequestSort,
    headCells,
  } = props;
  const createSortHandler = (property: string | number) => (event: React.MouseEvent<unknown>) => {
    onRequestSort(event, property);
  };

  return (
    <TableHead>
      <TableRow>
        {headCells.map((headCell: HeadCell) => (
          <StyledTableCell
            key={headCell.id}
            align="left"
            padding="default"
            sortDirection={orderBy === headCell.id ? order : false}
          >
            {orderBy ? (
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
            ) : (
              headCell.label
            )}
          </StyledTableCell>
        ))}
      </TableRow>
    </TableHead>
  );
}

interface TableProps {
  order?: Order;
  setOrder(key: Order): void;
  orderBy?: string;
  setOrderBy?(orderBy: any): void;
  selected: string[];
  setSelected(rows: string[]): void;
  page?: number;
  setPage(page: number): void;
  rowsPerPage?: number;
  setRowsPerPage(count: number): void;
  headCells: HeadCell[];
  rows: any[];
  footer?: string | ReactNode;
}

export const SimpleTable = (props: PropsWithChildren<TableProps>): JSX.Element => {
  const {
    order = 'asc',
    orderBy,
    selected = [],
    page = 0,
    rowsPerPage = 8,
    setOrder,
    setOrderBy,
    setSelected,
    setPage,
    setRowsPerPage,
    rows,
    footer,
    headCells,
    children,
  } = props;
  const classes = useStyles();
  const theme = useTheme();
  const handleRequestSort = (event: React.MouseEvent<unknown>, property: string | number) => {
    const isAsc = orderBy === property && order === 'asc';
    setOrder(isAsc ? 'desc' : 'asc');
    if (orderBy && setOrderBy) {
      setOrderBy(property);
    }
  };

  const handleSelectAllClick = (event: React.ChangeEvent<HTMLInputElement>) => {
    if (event.target.checked) {
      const newSelecteds = rows.map((n) => n.id);
      setSelected(newSelecteds);
      return;
    }
    setSelected([]);
  };

  const handleChangePage = (event: unknown, newPage: number) => {
    setPage(newPage);
  };

  const handleChangeRowsPerPage = (event: React.ChangeEvent<HTMLInputElement>) => {
    setRowsPerPage(parseInt(event.target.value, DECIMAL_RADIX));
    setPage(0);
  };

  return (
    <React.Fragment>
      <Grid item>
        <TableContainer className={classes.tableContainer}>
          <Table
            className={classes.table}
            aria-labelledby="tableTitle"
            size="medium"
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
              headCells={headCells}
            />
            <TableBody>{children}</TableBody>
          </Table>
        </TableContainer>
      </Grid>
      <Grid container direction="row" justify="space-between">
        <Grid item className={classes.footer}>
          {footer}
        </Grid>
        <TablePagination
          rowsPerPageOptions={[8]}
          component="div"
          count={rows.length}
          rowsPerPage={rowsPerPage}
          page={page}
          onChangePage={handleChangePage}
          onChangeRowsPerPage={handleChangeRowsPerPage}
        />
      </Grid>
    </React.Fragment>
  );
};

export default SimpleTable;

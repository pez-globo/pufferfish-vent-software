import React, { useCallback, useEffect } from 'react';
import { createStyles, makeStyles, Theme, useTheme } from '@material-ui/core/styles';
import { Grid, TableCell, TableRow, Typography } from '@material-ui/core';
import { useDispatch, useSelector } from 'react-redux';
import SimpleTable, {
  stableSort,
  getComparator,
  StyledTableRow,
  Order,
  HeadCell,
} from '../controllers/SimpleTable';
import ModalPopup from '../controllers/ModalPopup';
import EventlogDetails from './container/EventlogDetails';
import {
  getActiveLogEventIds,
  getLogEvent,
  getNextLogEvents,
} from '../../store/controller/selectors';
import { LogEvent } from '../../store/controller/proto/mcu_pb';
import { getEventType } from '../app/EventAlerts';
import { updateCommittedState } from '../../store/controller/actions';
import { EXPECTED_LOG_EVENT_ID } from '../../store/controller/types';

/**
 * LogsPage
 *
 * TODO: Abstract this into components!
 */

interface Data {
  type: string;
  alarm: string;
  time: number; // Note: Make this a date object?
  status: string;
  id: number;
}

//

const headCells: HeadCell[] = [
  { id: 'type', numeric: false, disablePadding: true, label: 'Type' },
  { id: 'alarm', numeric: true, disablePadding: false, label: 'Alarm' },
  { id: 'time', numeric: true, disablePadding: false, label: 'Time/Date' },
  { id: 'Status', numeric: false, disablePadding: false, label: 'Status' },
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
  const dispatch = useDispatch();

  const createData = (
    type: string,
    alarm: string,
    time: number,
    status: string,
    id: number,
  ): Data => {
    return { type, alarm, time, status, id };
  };

  const [rows, setRows] = React.useState<Data[]>([]);
  const [order, setOrder] = React.useState<Order>('desc');
  const [orderBy, setOrderBy] = React.useState<keyof Data>('time');
  const [selected, setSelected] = React.useState<string[]>([]);
  const [page, setPage] = React.useState(0);
  const [rowsPerPage, setRowsPerPage] = React.useState(8);
  const [open, setOpen] = React.useState(false);
  const [currentRow, setCurrentRow] = React.useState<Data>();
  const isSelected = (name: string) => selected.indexOf(name) !== -1;
  const emptyRows = rowsPerPage - Math.min(rowsPerPage, rows.length - page * rowsPerPage);
  const loggedEvents = useSelector(getNextLogEvents);
  const activeLogEventIds = useSelector(getActiveLogEventIds);

  const updateLogEvent = useCallback(
    (maxId) => {
      dispatch(updateCommittedState(EXPECTED_LOG_EVENT_ID, { id: maxId + 1 }));
    },
    [dispatch],
  );

  useEffect(() => {
    const eventIds: number[] = [];
    loggedEvents.sort((a: LogEvent, b: LogEvent) => a.time - b.time);
    const data = loggedEvents.map((event: LogEvent) => {
      const eventType = getEventType(event.code);
      const diffString =
        event.oldValue && event.newValue
          ? `(${event.oldValue} ${eventType.unit} to ${event.newValue} ${eventType.unit})`
          : '';
      eventIds.push(event.id);
      return createData(
        eventType.type,
        `${eventType.label} ${diffString}`,
        event.time,
        activeLogEventIds.indexOf(event.id) > -1 ? 'Active' : 'In Active',
        event.id,
      );
    });
    setRows(data);
    // update ExpectedLogEvent
    updateLogEvent(Math.max(...eventIds));
  }, [loggedEvents, activeLogEventIds, updateLogEvent]);

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
                key={row.id}
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
                <TableCell align="left" component="th" scope="row">
                  {row.status}
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

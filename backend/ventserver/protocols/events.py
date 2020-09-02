"""Sans-I/O protocol event utilities."""

import abc


class Event(abc.ABC):
    """Interface for Sans-I/O protocol event classes."""

    @abc.abstractmethod
    def has_data(self) -> bool:
        """Return whether the event has data."""

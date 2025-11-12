// Filter functionality for the project library
document.addEventListener('DOMContentLoaded', function() {
    const typeFilter = document.getElementById('type-filter');
    const softwareFilter = document.getElementById('software-filter');
    const dateFilter = document.getElementById('date-filter');
    const clearFiltersBtn = document.getElementById('clear-filters');
    const projectsGrid = document.getElementById('projects-grid');
    const projectCards = projectsGrid.querySelectorAll('.project-card');

    function filterProjects() {
        const selectedType = typeFilter.value;
        const selectedSoftware = softwareFilter.value;
        const selectedDate = dateFilter.value;

        projectCards.forEach(card => {
            const cardType = card.dataset.type;
            const cardSoftware = card.dataset.software;
            const cardDate = new Date(card.dataset.date);

            let showCard = true;

            // Filter by type
            if (selectedType !== 'all' && cardType !== selectedType) {
                showCard = false;
            }

            // Filter by software
            if (selectedSoftware !== 'all' && !cardSoftware.includes(selectedSoftware)) {
                showCard = false;
            }

            // Sort by date
            if (selectedDate === 'newest') {
                // Sort cards by date descending
                const sortedCards = Array.from(projectCards).sort((a, b) => {
                    return new Date(b.dataset.date) - new Date(a.dataset.date);
                });
                sortedCards.forEach(card => projectsGrid.appendChild(card));
            } else if (selectedDate === 'oldest') {
                // Sort cards by date ascending
                const sortedCards = Array.from(projectCards).sort((a, b) => {
                    return new Date(a.dataset.date) - new Date(b.dataset.date);
                });
                sortedCards.forEach(card => projectsGrid.appendChild(card));
            }

            // Show/hide card based on filters
            card.style.display = showCard ? 'block' : 'none';
        });
    }

    function clearFilters() {
        typeFilter.value = 'all';
        softwareFilter.value = 'all';
        dateFilter.value = 'newest';
        filterProjects();
    }

    // Event listeners
    typeFilter.addEventListener('change', filterProjects);
    softwareFilter.addEventListener('change', filterProjects);
    dateFilter.addEventListener('change', filterProjects);
    clearFiltersBtn.addEventListener('click', clearFilters);

    // Initial filter application
    filterProjects();
});

// Smooth scrolling for navigation
document.querySelectorAll('a[href^="#"]').forEach(anchor => {
    anchor.addEventListener('click', function (e) {
        e.preventDefault();
        document.querySelector(this.getAttribute('href')).scrollIntoView({
            behavior: 'smooth'
        });
    });
});

// Slide-in animation for project cards on scroll
document.addEventListener('DOMContentLoaded', function() {
    const librarySection = document.getElementById('library');
    const projectCards = document.querySelectorAll('.project-card');

    const observer = new IntersectionObserver((entries) => {
        entries.forEach(entry => {
            if (entry.isIntersecting) {
                projectCards.forEach((card, index) => {
                    setTimeout(() => {
                        card.classList.add('slide-in');
                    }, index * 200); // Stagger the animation by 200ms per card
                });
                observer.unobserve(entry.target); // Stop observing once animation is triggered
            }
        });
    }, {
        threshold: 0.1 // Trigger when 10% of the section is visible
    });

    observer.observe(librarySection);
});

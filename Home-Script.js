function scrollToTop() {
  window.scrollTo({
    top: 0,
    behavior: 'smooth'
  });
}

// Scroll animation for header
window.addEventListener('scroll', () => {
  const scrollY = window.scrollY;
  const maxScroll = 500; // max scroll value to trigger full animation

  // Calculate scroll progress between 0 and 1
  const scrollProgress = Math.min(scrollY / maxScroll, 1);

  // Calculate new header height
  const header = document.querySelector('.header');
  const titleContainer = document.querySelector('.title-container');

  // Interpolate header height from 1000px to 200px
  const newHeaderHeight = 1000 - (800 * scrollProgress);
  header.style.height = newHeaderHeight + 'px';

});

document.addEventListener('DOMContentLoaded', () => {
  const aboutMe = document.querySelector('.about-me');
  const projectArea = document.querySelector('.project-area');
  const cardWraps = document.querySelectorAll('.cardWrap');

  const observerOptions = {
    root: null,
    rootMargin: '0px',
    threshold: 0.2
  };

  const observerCallback = (entries) => {
    entries.forEach(entry => {
      const target = entry.target;
      if (entry.isIntersecting) {
        if (target === projectArea) {
          cardWraps.forEach(card => card.classList.add('animate-in'));
          const line = document.querySelector('.animated-line');
          if (line) line.classList.add('animate');
        }
      } else {
        if (target === projectArea) {
          cardWraps.forEach(card => card.classList.remove('animate-in'));
          const line = document.querySelector('.animated-line');
          if (line) line.classList.remove('animate');
        }
      }
    });
  };

  const observer = new IntersectionObserver(observerCallback, observerOptions);
  if (aboutMe) observer.observe(aboutMe);
  if (projectArea) observer.observe(projectArea);

  // New code for filtering and sorting projects
  const projectsGrid = document.getElementById('projects-grid');
  const typeFilter = document.getElementById('type-filter');
  const toolFilter = document.getElementById('tool-filter');
  const sortSelect = document.getElementById('sort-select');
  const clearFiltersBtn = document.getElementById('clear-filters');

  function filterAndSortProjects() {
    const selectedType = typeFilter.value;
    const selectedTool = toolFilter.value;
    const sortBy = sortSelect.value;

    let projects = Array.from(projectsGrid.querySelectorAll('.cardWrap'));

    // Filter projects
    projects.forEach(project => {
      const type = project.getAttribute('data-type');
      const tools = project.getAttribute('data-tools').split(',');
      const matchesType = selectedType === 'All' || type === selectedType;
      const matchesTool = selectedTool === 'All' || tools.includes(selectedTool);

      if (matchesType && matchesTool) {
        project.style.display = '';
      } else {
        project.style.display = 'none';
      }
    });

    // Sort projects
    projects = projects.filter(p => p.style.display !== 'none');

    projects.sort((a, b) => {
      if (sortBy === 'date-desc') {
        return b.getAttribute('data-date').localeCompare(a.getAttribute('data-date'));
      } else if (sortBy === 'date-asc') {
        return a.getAttribute('data-date').localeCompare(b.getAttribute('data-date'));
      } else if (sortBy === 'name') {
        const aName = a.querySelector('h3').textContent;
        const bName = b.querySelector('h3').textContent;
        return aName.localeCompare(bName);
      }
      return 0;
    });

    // Reorder DOM elements
    projects.forEach(project => projectsGrid.appendChild(project));
  }

  typeFilter.addEventListener('change', filterAndSortProjects);
  toolFilter.addEventListener('change', filterAndSortProjects);
  sortSelect.addEventListener('change', filterAndSortProjects);
  clearFiltersBtn.addEventListener('click', () => {
    typeFilter.value = 'All';
    toolFilter.value = 'All';
    sortSelect.value = 'date-desc';
    filterAndSortProjects();
  });

  // Initial filter and sort
  filterAndSortProjects();
});


